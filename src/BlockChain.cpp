#include "../include/BlockChain.h"
#include <limits>
#include <iomanip>
#include <map>


// Конструктор
BlockChain::BlockChain(const std::string& name, const std::string& saveFileName, std::vector<User*>* users)
    : Name(name), SaveFileName(saveFileName), GenesisBlock(nullptr), LastBlock(nullptr), Burse(nullptr), CountBlocks(0), AllUsers(users) {
    TransactionsPool = new Pool();
    std::cout << "Blockchain '" << Name << "' system initiated.\n";
}

// Деструктор
BlockChain::~BlockChain() {
    delete TransactionsPool;
    clearChain();
    std::cout << "Blockchain '" << Name << "' system shut down.\n";
}


void BlockChain::clearChain() {
    Block* current = GenesisBlock;
    while (current != nullptr) {
        Block* next = current->GetNextBlock();
        delete current;
        current = next;
    }
    GenesisBlock = nullptr;
    LastBlock = nullptr;
    CountBlocks = 0;
}

void BlockChain::CreateGenesisBlock(const std::vector<Transaction*>& initialTransactions) {
    if (GenesisBlock) {
        std::cerr << "Genesis block already exists!\n";
        return;
    }

    GenesisBlock = new Block(0, nullptr, initialTransactions);
    LastBlock = GenesisBlock;
    CountBlocks = 1;
    std::cout << "Genesis Block created with hash: " << GenesisBlock->GetHash() << std::endl;

    // Сохраняем сразу после создания генезис-блока
    saveToFile();
}

bool BlockChain::IsTransactionValid(Transaction* trans) const {
    if (!trans) return false;

    if (trans->GetSender() == Burse) {
        return true;
    }

    double senderBalance = GetUserBalance(trans->GetSender());
    if (senderBalance < trans->GetAmount()) {
        std::cerr << "Validation Error: Sender " << trans->GetSender()->GetName()
                  << " has insufficient funds (" << senderBalance << ") for transaction amount "
                  << trans->GetAmount() << std::endl;
        return false;
    }

    return true;
}

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
        Transaction* trans = TransactionsPool->TopTrans();
        TransactionsPool->DeleteTrans();

        if (IsTransactionValid(trans)) {
            transactionsToMine.push_back(trans);
        } else {
            std::cerr << "Validation Error: Transaction " << trans->ToString()
                      << " is invalid and will be discarded.\n";
            delete trans;
        }
        processedCount++;
    }

    if (transactionsToMine.empty() && CountBlocks > 0) {
        std::cout << "No valid transactions found to mine a block.\n";
        return false;
    }

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

    if (saveToFile()) {
        std::cout << "Blockchain state automatically saved.\n";
    } else {
        std::cerr << "Warning: Failed to automatically save blockchain state.\n";
    }

    return true;
}

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

// Сохранение блокчейна в файл
bool BlockChain::saveToFile() const {
    std::ofstream outFile(SaveFileName, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error: Could not open file for writing: " << SaveFileName << std::endl;
        return false;
    }

    const std::vector<User*>& users = *AllUsers;
    size_t userCount = users.size();
    outFile.write(reinterpret_cast<const char*>(&userCount), sizeof(userCount));
    for (const auto& user : users) {
        int id = user->GetID();
        std::string name = user->GetName();
        size_t nameLen = name.length();
        outFile.write(reinterpret_cast<const char*>(&id), sizeof(id));
        outFile.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
        outFile.write(name.c_str(), nameLen);
    }

    outFile.write(reinterpret_cast<const char*>(&CountBlocks), sizeof(CountBlocks));
    Block* current = GenesisBlock;
    while (current != nullptr) {
        int num = current->GetNumber();
        time_t ts = current->GetTimeStamp();
        std::string hash = current->GetHash();
        size_t hashLen = hash.length();
        std::string merkle = current->GetHashMerkle();
        size_t merkleLen = merkle.length();

        outFile.write(reinterpret_cast<const char*>(&num), sizeof(num));
        outFile.write(reinterpret_cast<const char*>(&ts), sizeof(ts));
        outFile.write(reinterpret_cast<const char*>(&hashLen), sizeof(hashLen));
        outFile.write(hash.c_str(), hashLen);
        outFile.write(reinterpret_cast<const char*>(&merkleLen), sizeof(merkleLen));
        outFile.write(merkle.c_str(), merkleLen);

        const auto& transactions = current->GetTransactionList();
        size_t transCount = transactions.size();
        outFile.write(reinterpret_cast<const char*>(&transCount), sizeof(transCount));

        for (const auto& trans : transactions) {
            int senderId = trans->GetSender() ? trans->GetSender()->GetID() : 0; // 0 для фонда
            int receiverId = trans->GetReceiver() ? trans->GetReceiver()->GetID() : 0;
            double amount = trans->GetAmount();
            time_t transTs = trans->GetTimeStamp();

            outFile.write(reinterpret_cast<const char*>(&senderId), sizeof(senderId));
            outFile.write(reinterpret_cast<const char*>(&receiverId), sizeof(receiverId));
            outFile.write(reinterpret_cast<const char*>(&amount), sizeof(amount));
            outFile.write(reinterpret_cast<const char*>(&transTs), sizeof(transTs));
        }
        current = current->GetNextBlock();
    }

    outFile.close();
    return true;
}

// Загрузка блокчейна из файла
bool BlockChain::loadFromFile() {
    std::ifstream inFile(SaveFileName, std::ios::binary);
    if (!inFile) {
        return false;
    }

    clearChain();
    AllUsers->clear();
    std::map<int, User*> userMap;
    userMap[Burse->GetID()] = Burse;

    size_t userCount;
    inFile.read(reinterpret_cast<char*>(&userCount), sizeof(userCount));
    if (inFile.fail()) return false;
    for (size_t i = 0; i < userCount; ++i) {
        int id;
        size_t nameLen;
        inFile.read(reinterpret_cast<char*>(&id), sizeof(id));
        inFile.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
        if (inFile.fail()) return false;
        std::string name(nameLen, '\0');
        inFile.read(&name[0], nameLen);
        if (inFile.fail()) return false;

        User* newUser = new User(id, name, this);
        AllUsers->push_back(newUser);
        userMap[id] = newUser;
    }

    inFile.read(reinterpret_cast<char*>(&CountBlocks), sizeof(CountBlocks));
    if (inFile.fail()) return false;
    for (int i = 0; i < CountBlocks; ++i) {
        int num;
        time_t ts;
        size_t hashLen, merkleLen;

        inFile.read(reinterpret_cast<char*>(&num), sizeof(num));
        inFile.read(reinterpret_cast<char*>(&ts), sizeof(ts));

        inFile.read(reinterpret_cast<char*>(&hashLen), sizeof(hashLen));
        if (inFile.fail()) return false;
        std::string hash(hashLen, '\0');
        inFile.read(&hash[0], hashLen);

        inFile.read(reinterpret_cast<char*>(&merkleLen), sizeof(merkleLen));
        if (inFile.fail()) return false;
        std::string merkle(merkleLen, '\0');
        inFile.read(&merkle[0], merkleLen);
        if (inFile.fail()) return false;

        std::vector<Transaction*> loadedTransactions;
        size_t transCount;
        inFile.read(reinterpret_cast<char*>(&transCount), sizeof(transCount));
        if (inFile.fail()) return false;
        for (size_t j = 0; j < transCount; ++j) {
            int senderId, receiverId;
            double amount;
            time_t transTs;

            inFile.read(reinterpret_cast<char*>(&senderId), sizeof(senderId));
            inFile.read(reinterpret_cast<char*>(&receiverId), sizeof(receiverId));
            inFile.read(reinterpret_cast<char*>(&amount), sizeof(amount));
            inFile.read(reinterpret_cast<char*>(&transTs), sizeof(transTs));
            if (inFile.fail()) return false;

            User* sender = userMap[senderId];
            User* receiver = userMap[receiverId];

            loadedTransactions.push_back(new Transaction(sender, receiver, amount, transTs));
        }

        Block* newBlock = new Block(num, ts, hash, merkle, "", LastBlock, loadedTransactions);

        if (!GenesisBlock) {
            GenesisBlock = newBlock;
        }
        if (LastBlock) {
            LastBlock->SetNextBlock(newBlock);
        }
        LastBlock = newBlock;
    }

    inFile.close();
    std::cout << "Blockchain state loaded from " << SaveFileName << std::endl;
    return true;
}