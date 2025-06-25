#ifndef BLOCK_H
#define BLOCK_H

#include <string>
#include <vector>
#include <iostream>
#include "Transaction.h"
#include "sha256.h"

class Block {
private:
    int Number;
    std::string Hash;
    std::string HashMerkle;
    Block* PrevBlock;
    Block* NextBlock;
    time_t TimeStamp;
    std::vector<Transaction*> TransactionList;
    std::string Nonce;

public:
    // Конструктор для создания нового блока
    Block(int number, Block* prevBlock, const std::vector<Transaction*>& transactions);
    // Новый конструктор для загрузки блока из файла
    Block(int number, time_t timestamp, std::string hash, std::string hashMerkle, std::string nonce, Block* prevBlock, const std::vector<Transaction*>& transactions);

    // Геттеры
    std::string GetHash() const { return Hash; }
    int GetNumber() const { return Number; }
    Block* GetPrevBlock() const { return PrevBlock; }
    Block* GetNextBlock() const { return NextBlock; }
    time_t GetTimeStamp() const { return TimeStamp; }
    const std::vector<Transaction*>& GetTransactionList() const { return TransactionList; }
    std::string GetHashMerkle() const { return HashMerkle; }

    // Сеттер
    void SetNextBlock(Block* nextBlock) { NextBlock = nextBlock; }

    // Методы
    std::string CalculateHash();
    static std::string CalculateMerkleRoot(const std::vector<Transaction*>& transactions);
    std::string ToString() const;

    // Деструктор
    ~Block();
};

#endif // BLOCK_H