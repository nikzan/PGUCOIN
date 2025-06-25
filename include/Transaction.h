#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <iostream>
#include <string>
#include <ctime>

enum Action {
    DEPOSIT, // Пополнение баланса
    WITHDRAW, // Снятие с баланса
    TRANSFER // Перевод средств между пользователями
};

class User;

class Transaction {
private:
    User* Sender;
    User* Receiver;
    double Amount;
    time_t TimeStamp;

public:
    // Конструктор для новых транзакций
    Transaction(User* SetSender, User* SetReceiver, double SetAmount);
    // Конструктор для загрузки из файла
    Transaction(User* SetSender, User* SetReceiver, double SetAmount, time_t timestamp);

    // Геттеры
    User* GetSender() const { return Sender; }
    User* GetReceiver() const { return Receiver; }
    double GetAmount() const { return Amount; }
    time_t GetTimeStamp() const { return TimeStamp; }

    std::string ToString() const;
};

#endif // TRANSACTION_H