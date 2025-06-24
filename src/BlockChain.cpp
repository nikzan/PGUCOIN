#include "../include/BlockChain.h"
#include <limits>
#include <iomanip>

BlockChain::BlockChain(const std::string& name) : Name(name), GenesisBlock(nullptr), LastBlock(nullptr), Burse(nullptr), CountBlocks(0) {
    TransactionsPool = new Pool();
    std::cout << "Blockchain '" << Name << "' created.\n";
}

// Деструктор
BlockChain::~BlockChain() {
    delete TransactionsPool;
    Block* current = GenesisBlock;
    while (current != nullptr) {
        Block* next = current->GetNextBlock();
        delete current;
        current = next;
    }
    GenesisBlock = nullptr;
    LastBlock = nullptr;
    std::cout << "Blockchain '" << Name << "' destroyed.\n";
}

// Добавил создание генезис-блока
void BlockChain::CreateGenesisBlock(const std::vector<Transaction*>& initialTransactions) {
    if (GenesisBlock) {
        std::cerr << "Genesis block already exists!\n";
        return;
    }

    GenesisBlock = new Block(0, nullptr, initialTransactions);
    LastBlock = GenesisBlock;
    CountBlocks = 1;
    std::cout << "Genesis Block created with hash: " << GenesisBlock->GetHash() << std::endl;
}

// Проверка транзакции, чтобы блок ее не майнил, если она невалидна
bool BlockChain::IsTransactionValid(Transaction* trans) const {
    if (!trans) return false;

    // Добавил Burse, транзакции депозита приходят,
    // как перевод от него (баланс фонда может уйти в минус, его транзакции ВСЕГДА валидны)
    if (trans->GetSender() == Burse) {
        return true;
    }

    // Проверка, что баланс пользователя (кроме фонда), позволяет выполнить транзакцию
    double senderBalance = GetUserBalance(trans->GetSender());
    if (senderBalance < trans->GetAmount()) {
        std::cerr << "Validation Error: Sender " << trans->GetSender()->GetName()
                  << " has insufficient funds (" << senderBalance << ") for transaction amount "
                  << trans->GetAmount() << std::endl;
        return false;
    }

    return true;
}

// Майним новый блок с транзакциями из пула
bool BlockChain::MineBlock(int maxTransactions) {
    if (TransactionsPool->IsEmpty() && CountBlocks > 0) {
        std::cout << "No transactions in the pool to mine.\n";
        return false;
    }

    if (!GenesisBlock) {
        std::cout << "Creating Genesis Block first...\n";
        CreateGenesisBlock({});
        return true;
    }

    std::cout << "\nAttempting to mine a new block...\n";
    std::vector<Transaction*> transactionsToMine;

    int processedCount = 0;
    while (!TransactionsPool->IsEmpty() && processedCount < maxTransactions) {

        // Берем транзакцию из очереди пула
        Transaction* trans = TransactionsPool->TopTrans();
        TransactionsPool->DeleteTrans();

        // Проверяем на валидность
        if (IsTransactionValid(trans)) {
            transactionsToMine.push_back(trans);
        } else {
            // Исключаем транзакции, непрошедшие проверку
            std::cerr << "Validation Error: Transaction " << trans->ToString()
                      << " is invalid and will be discarded.\n";
            delete trans;
        }
        processedCount++;
    }

    if (transactionsToMine.empty()) {
        std::cout << "No valid transactions found to mine a block.\n";
        return false;
    }

    // Новые блок с учетом майненых транзакций
    Block* newBlock = new Block(CountBlocks, LastBlock, transactionsToMine);

    if (LastBlock) {
        LastBlock->SetNextBlock(newBlock);
    }
    LastBlock = newBlock;
    CountBlocks++;

    std::cout << "Block #" << newBlock->GetNumber() << " mined!" << std::endl;
    std::cout << "  Hash: " << newBlock->GetHash() << std::endl;
    std::cout << "  Merkle Root: " << newBlock->GetHashMerkle() << std::endl;
    std::cout << "  Transactions: " << newBlock->GetTransactionList().size() << std::endl;
    std::cout << "  Pool size remaining: " << TransactionsPool->GetSize() << std::endl;

    return true;
}

// Изменяем баланс пользователей
double BlockChain::GetUserBalance(const User* user) const {
    if (!user) return 0.0;
    double balance = 0.0;

    Block* current = GenesisBlock;
    while (current != nullptr) {
        for (const auto& trans : current->GetTransactionList()) {
            if (trans->GetReceiver() == user) {
                balance += trans->GetAmount();
            }
            if (trans->GetSender() == user) {
                balance -= trans->GetAmount();
            }
        }
        current = current->GetNextBlock();
    }
    return balance;
}

void BlockChain::PrintChain() const {
    std::cout << "\n--- BLOCKCHAIN: " << Name << " ---\n";
    if (!GenesisBlock) {
        std::cout << "Blockchain is empty.\n";
        return;
    }

    Block* current = GenesisBlock;
    while (current != nullptr) {
        std::cout << "Block #" << current->GetNumber() << "\n";
        std::cout << "  Hash: " << current->GetHash() << "\n";
        std::cout << "  Previous Hash: " << (current->GetPrevBlock() ? current->GetPrevBlock()->GetHash() : "N/A (Genesis)") << "\n";
        std::cout << "  Merkle Root: " << current->GetHashMerkle() << "\n";
        time_t timestamp_val = current->GetTimeStamp();
        std::cout << "  Timestamp: " << ctime(&timestamp_val);
        std::cout << "  Transactions (" << current->GetTransactionList().size() << "):" << "\n";
        for (const auto& trans : current->GetTransactionList()) {
            std::cout << "    - From: " << (trans->GetSender() ? trans->GetSender()->GetName() : "Burse")
                      << " To: " << (trans->GetReceiver() ? trans->GetReceiver()->GetName() : "Burse")
                      << " Amount: " << std::fixed << std::setprecision(2) << trans->GetAmount() << "\n";
        }
        std::cout << "\n";
        current = current->GetNextBlock();
    }
    std::cout << "--- END OF CHAIN ---\n";
}