#include "../include/Transaction.h"
#include "../include/User.h"
#include <sstream>

Transaction::Transaction(User* SetSender, User* SetReceiver, double SetAmount) :
    Sender(SetSender), Receiver(SetReceiver), Amount(SetAmount) {
    TimeStamp = time(nullptr);
}

std::string Transaction::ToString() const {
    std::stringstream ss;
    // Формируем строку с информацией о транзакции (для хеширования)
    ss << (Sender ? std::to_string(Sender->GetID()) : "NULL_SENDER") << ":"
       << (Receiver ? std::to_string(Receiver->GetID()) : "NULL_RECEIVER") << ":"
       << Amount << ":"
       << TimeStamp;
    return ss.str();
}