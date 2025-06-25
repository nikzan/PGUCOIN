#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <fstream>

#include "Block.h"
#include "Pool.h"
#include "User.h"

class BlockChain {
private:
    Block* GenesisBlock;
    Block* LastBlock;
    Pool* TransactionsPool;
    User* Burse;

    std::string Name;
    std::string SaveFileName;
    int CountBlocks;
    std::vector<User*>* AllUsers;

    void clearChain();
    bool IsTransactionValid(Transaction* trans) const;
    bool saveToFile() const;

public:
    // Конструктор принимает имя файла и указатель на вектор пользователей
    BlockChain(const std::string& name, const std::string& saveFileName, std::vector<User*>* users);
    ~BlockChain();

    // Геттеры
    Pool* GetPool() const { return TransactionsPool; }
    Block* GetGenesisBlock() const { return GenesisBlock; }
    Block* GetLastBlock() const { return LastBlock; }
    User* GetBurse() const { return Burse; }
    int GetCountBlocks() const { return CountBlocks; }

    // Сеттеры
    void SetBurse(User* setBurse) { Burse = setBurse; }

    void CreateGenesisBlock(const std::vector<Transaction*>& initialTransactions = {});
    bool MineBlock(int maxTransactions = 10);
    double GetUserBalance(const User* user) const;
    void PrintChain() const;

    // Метод загрузки
    bool loadFromFile();
};

#endif