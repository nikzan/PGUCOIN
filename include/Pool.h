#ifndef POOL_H
#define POOL_H

#include <queue>
#include "Transaction.h"

class Pool {
private:
    std::queue<Transaction*> TransPool;
public:
    void AddTrans(Transaction* AddedTrans) { TransPool.push(AddedTrans); }
    void DeleteTrans() { TransPool.pop(); }
    Transaction* TopTrans() { return TransPool.front(); }
    bool IsEmpty() const { return TransPool.empty(); }

    // Геттер
    size_t GetSize() const { return TransPool.size(); }

    std::vector<Transaction*> GetTransactions(size_t count);
    void RemoveTransactions(size_t count);

    ~Pool() {
        while (!TransPool.empty()) {
            Transaction* trans = TransPool.front();
            delete trans;
            TransPool.pop();
        }
    }
};

#endif