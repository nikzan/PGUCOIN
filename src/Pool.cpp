#include "../include/Pool.h"
#include <algorithm>

std::vector<Transaction*> Pool::GetTransactions(size_t count) {
    std::vector<Transaction*> transactions;
    size_t numToGet = std::min(count, TransPool.size());
    std::queue<Transaction*> tempPool = TransPool;
    for (size_t i = 0; i < numToGet; ++i) {
        transactions.push_back(tempPool.front());
        tempPool.pop();
    }
    return transactions;
}

void Pool::RemoveTransactions(size_t count) {
    size_t numToRemove = std::min(count, TransPool.size());
    for (size_t i = 0; i < numToRemove; ++i) {
        Transaction* trans = TransPool.front();
        delete trans;
        TransPool.pop();
    }
}