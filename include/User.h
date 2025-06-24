#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include "Transaction.h"

class BlockChain;

class User {
private:
    int UserID;
    std::string Name;
    BlockChain* HisBlockChain;
    std::vector<Transaction*> TransHistory;

public:
    User();
    User(int id, const std::string& name, BlockChain* bc);

    std::string GetName() const { return Name; }
    void SetName(std::string NewName) { Name = NewName; }

    void SetID(int Number) { UserID = Number; }
    int GetID() const { return UserID; }

    void SetBlockChain(BlockChain* NewBlockChain) { HisBlockChain = NewBlockChain; }
    BlockChain* GetBlockChain() const { return HisBlockChain; }

    bool MakeTransaction(User* Person, double Amount, Action Oper);

    ~User();
};

#endif // USER_H