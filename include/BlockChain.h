#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

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
    int CountBlocks;

    bool IsTransactionValid(Transaction* trans) const;

public:
    BlockChain(const std::string& name);
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
};

#endif