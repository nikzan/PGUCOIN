#include "../include/Transaction.h"
#include "../include/User.h"
#include <sstream>

// Основной конструктор
Transaction::Transaction(User* SetSender, User* SetReceiver, double SetAmount) :
    Sender(SetSender), Receiver(SetReceiver), Amount(SetAmount) {
  TimeStamp = time(nullptr);
}

// Конструктор по умолчанию (для загрузки)
Transaction::Transaction(User* SetSender, User* SetReceiver, double SetAmount, time_t timestamp) :
    Sender(SetSender), Receiver(SetReceiver), Amount(SetAmount), TimeStamp(timestamp) {
}

std::string Transaction::ToString() const {
  std::stringstream ss;
  ss << (Sender ? std::to_string(Sender->GetID()) : "NULL_SENDER") << ":"
     << (Receiver ? std::to_string(Receiver->GetID()) : "NULL_RECEIVER") << ":"
     << Amount << ":"
     << TimeStamp;
  return ss.str();
}