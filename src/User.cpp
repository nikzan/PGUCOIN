#include "../include/User.h"
#include "../include/BlockChain.h"
#include "../include/Pool.h"
#include <iostream>

User::User() : UserID(0), Name(""), HisBlockChain(nullptr) {} // Конструктор по умолчанию для обработки пустых пользователей

User::User(int id, const std::string& name, BlockChain* bc) : UserID(id), Name(name), HisBlockChain(bc) {} // Основной конструктор

bool User::MakeTransaction(User* Person, double Amount, Action Oper) {
    if (!HisBlockChain || !HisBlockChain->GetPool() || !HisBlockChain->GetBurse()) {
        std::cerr << "Error: Blockchain or Pool or Burse not properly set for user " << Name << ".\n";
        return false;
    }

    if (Amount <= 0) {
        std::cerr << "Error: Transaction amount must be positive.\n";
        return false;
    }

    Transaction* pTrans = nullptr;
    bool success = true;

    switch (Oper) {

        // Операции с фондом

        case Action::DEPOSIT:
            if (Person != this) {
                std::cerr << "Error: Deposit must be to self.\n";
                success = false;
            } else {
                pTrans = new Transaction(HisBlockChain->GetBurse(), this, Amount);
                HisBlockChain->GetPool()->AddTrans(pTrans);
            }
            break;
        case Action::WITHDRAW:
            if (Person != this) {
                std::cerr << "Error: Withdrawal must be from self.\n";
                success = false;
            } else {
                // Смотрим по последнему блоку, для улучшения безопасности можем допилить локальную проверку в будущем
                pTrans = new Transaction(this, HisBlockChain->GetBurse(), Amount);
                HisBlockChain->GetPool()->AddTrans(pTrans);
            }
            break;

        // Операции между пользователями

        case Action::TRANSFER:
            if (this == Person) {
                std::cerr << "Error: Cannot transfer to self.\n";
                success = false;
            } else if (!Person) {
                 std::cerr << "Error: Receiver is null.\n";
                success = false;
            }
            else {
                pTrans = new Transaction(this, Person, Amount);
                HisBlockChain->GetPool()->AddTrans(pTrans);
            }
            break;

        default:
            std::cerr << "Error: Unknown action type.\n";
            success = false;
            break;
    }

    if (pTrans && success) {
        TransHistory.push_back(pTrans);

    } else {
        delete pTrans; // Удаляем невалидную транзакцию, чтобы избежать утечки
    }
    return success;
}

User::~User() {
    TransHistory.clear();
}