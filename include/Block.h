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
    std::string Nonce; // Добавил для демонстрации PoW (пока тут не используем, на будущее)

public:
    // Конструктор
    Block(int number, Block* prevBlock, const std::vector<Transaction*>& transactions);

    // Геттеры
    std::string GetHash() const { return Hash; }
    int GetNumber() const { return Number; } // Номер блока для вывода в майнинге
    Block* GetPrevBlock() const { return PrevBlock; }
    Block* GetNextBlock() const { return NextBlock; }
    time_t GetTimeStamp() const { return TimeStamp; } // Время создания блока, для контроля за ним
    const std::vector<Transaction*>& GetTransactionList() const { return TransactionList; }
    std::string GetHashMerkle() const { return HashMerkle; } // Добавил для реализации Меркла

    // Сеттер
    void SetNextBlock(Block* nextBlock) { NextBlock = nextBlock; } // Для связи между блоками

    // Методы для вычисления хешей
    std::string CalculateHash();
    static std::string CalculateMerkleRoot(const std::vector<Transaction*>& transactions); // Считаем корень хеша Меркла (взял из статьи)
    std::string ToString() const;

    // Деструктор
    ~Block();
};

#endif // BLOCK_H