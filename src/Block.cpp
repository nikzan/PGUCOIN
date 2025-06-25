#include "../include/Block.h"
#include <sstream>
#include <algorithm>

// Основной конструктор
Block::Block(int number, Block* prevBlock, const std::vector<Transaction*>& transactions)
    : Number(number), PrevBlock(prevBlock), NextBlock(nullptr), TransactionList(transactions) {
    TimeStamp = time(nullptr);
    HashMerkle = CalculateMerkleRoot(TransactionList);
    Nonce = "";
    Hash = CalculateHash();
}

// Конструктор по умолчанию (для загрузки)
Block::Block(int number, time_t timestamp, std::string hash, std::string hashMerkle, std::string nonce, Block* prevBlock, const std::vector<Transaction*>& transactions)
    : Number(number), TimeStamp(timestamp), Hash(hash), HashMerkle(hashMerkle), Nonce(nonce), PrevBlock(prevBlock), NextBlock(nullptr), TransactionList(transactions) {
    // Здесь мы не вычисляем хеш, а присваиваем тот, что был сохранен
}


std::string Block::CalculateHash() {
    return sha256(ToString());
}

std::string Block::CalculateMerkleRoot(const std::vector<Transaction*>& transactions) {
    if (transactions.empty()) {
        return sha256("");
    }

    std::vector<std::string> hashes;
    for (const auto& trans : transactions) {
        hashes.push_back(sha256(trans->ToString()));
    }

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

std::string Block::ToString() const {
    std::stringstream ss;
    ss << Number
       << (PrevBlock ? PrevBlock->GetHash() : "0")
       << TimeStamp
       << HashMerkle
       << Nonce;
    return ss.str();
}

Block::~Block() {
    for (Transaction* trans : TransactionList) {
        delete trans;
    }
    TransactionList.clear();
}