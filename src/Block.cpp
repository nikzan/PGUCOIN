#include "../include/Block.h"
#include <sstream>
#include <algorithm>

Block::Block(int number, Block* prevBlock, const std::vector<Transaction*>& transactions)
    : Number(number), PrevBlock(prevBlock), NextBlock(nullptr), TransactionList(transactions) {
    TimeStamp = time(nullptr);
    HashMerkle = CalculateMerkleRoot(TransactionList); // Добавил Меркла
    Hash = CalculateHash();
    Nonce = ""; // Задел под PoW, позже доделаем
}

std::string Block::CalculateHash() {
    return sha256(ToString());
}

// Вычисляем корневой хеш Меркла (из статьи)
std::string Block::CalculateMerkleRoot(const std::vector<Transaction*>& transactions) {
    if (transactions.empty()) {
        return sha256("");
    }

    std::vector<std::string> hashes;
    for (const auto& trans : transactions) {
        hashes.push_back(sha256(trans->ToString()));
    }

    // Построение дерева Меркла
    while (hashes.size() > 1) {
        if (hashes.size() % 2 != 0) {
            hashes.push_back(hashes.back());
        }
        std::vector<std::string> newHashes;
        for (size_t i = 0; i < hashes.size(); i += 2) {
            newHashes.push_back(sha256(hashes[i] + hashes[i+1]));
        }
        hashes = newHashes;
    }
    return hashes[0];
}

// Метод для получения данных блока для хеширования (переводим в строку)
std::string Block::ToString() const {
    std::stringstream ss;
    ss << Number
       << (PrevBlock ? PrevBlock->GetHash() : "0")
       << TimeStamp
       << HashMerkle
       << Nonce; // Для PoW
    return ss.str();
}

Block::~Block() {
    for (Transaction* trans : TransactionList) {
        delete trans;
    }
    TransactionList.clear();
}