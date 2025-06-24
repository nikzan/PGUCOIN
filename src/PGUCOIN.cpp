#include <iostream>
#include <iomanip>
#include <ctime>
#include <limits>
#include <vector>
#include "../include/BlockChain.h"
#include "../include/AdditionalFunctions.h"
#include "../include/User.h"
#include "../include/Transaction.h"

std::vector<std::string> FirstNames;
std::vector<std::string> SecondNames;

void displayMenu() {
    std::cout << "\n--- PGUCOIN Menu ---\n";
    std::cout << "0. | Mine a Block | \n";
    std::cout << "1. Make a Deposit\n";
    std::cout << "2. Make a Withdrawal\n";
    std::cout << "3. Make a Transfer\n";
    std::cout << "4. View User Balances\n";
    std::cout << "5. View Blockchain\n";
    std::cout << "6. Exit\n";
    std::cout << "Enter your choice: ";
}

// Подсмотрел на стаке, удобно, чтобы не писать две одинаковые под инт и дабл
// Проверка ввода пользователя
template<typename T>
T getValidatedInput(const std::string& prompt) {
    T value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;
        if (std::cin.fail()) {
            std::cout << "Invalid input. Please enter a number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }
    }
}

// Вывод доступных пользователей и выбор
User* selectUser(const std::vector<User*>& users, const std::string& prompt) {
    if (users.empty()) {
        std::cout << "No users available.\n";
        return nullptr;
    }

    std::cout << prompt << "\n";
    for (size_t i = 0; i < users.size(); ++i) {
        std::cout << i + 1 << ". " << users[i]->GetName() << " (ID: " << users[i]->GetID() << ")\n";
    }

    int choice = getValidatedInput<int>("Enter user number: ");
    if (choice > 0 && choice <= users.size()) {
        return users[choice - 1];
    } else {
        std::cout << "Invalid user selection.\n";
        return nullptr;
    }
}

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    FirstNames = ReadNames("../data/FirstNames.txt");
    SecondNames = ReadNames("../data/SecondNames.txt");

    if (FirstNames.empty() || SecondNames.empty()) {
        std::cerr << "Error: Could not load names. Ensure FirstNames.txt and SecondNames.txt are in the correct directory.\n";
        return 1;
    }

    BlockChain* PGUCOIN = new BlockChain("PGUCOIN_Network");
    User* Burse = new User(0, "Burse", PGUCOIN);
    PGUCOIN->SetBurse(Burse);

    int CountOfUsers = getValidatedInput<int>("Enter number of initial users (default = 3): ");
    if (CountOfUsers <= 0) CountOfUsers = 3;

    std::vector<User*> Users;
    for (int i = 0; i < CountOfUsers; i++){
        std::string Name;
        do {
            Name = GenerationName(FirstNames, SecondNames);
        } while (!CheckUserName(Users, Name));

        User* PUser = new User(i + 1, Name, PGUCOIN);
        Users.push_back(PUser);
        std::cout << "User created: " << PUser->GetName() << " (ID: " << PUser->GetID() << ")\n";
    }

    PGUCOIN->CreateGenesisBlock({});

    int choice;
    do {
        displayMenu();
        choice = getValidatedInput<int>("");

        User* senderUser = nullptr;
        User* receiverUser = nullptr;
        double amount;
        bool transactionSuccess = false;

        switch (choice) {
            case 0: { // Майнинг блока
                PGUCOIN->MineBlock();
                break;
            }
            case 1: { // Депозит
                senderUser = selectUser(Users, "Select user for Deposit:");
                if (!senderUser) break;
                amount = getValidatedInput<double>("Enter deposit amount: ");
                transactionSuccess = senderUser->MakeTransaction(senderUser, amount, Action::DEPOSIT);
                if (transactionSuccess) {
                    std::cout << "Deposit transaction added to pool for " << senderUser->GetName() << ".\n";
                }
                break;
            }
            case 2: { // Вывод
                senderUser = selectUser(Users, "Select user for Withdrawal:");
                if (!senderUser) break;
                amount = getValidatedInput<double>("Enter withdrawal amount: ");
                transactionSuccess = senderUser->MakeTransaction(senderUser, amount, Action::WITHDRAW);
                if (transactionSuccess) {
                    std::cout << "Withdrawal transaction added to pool for " << senderUser->GetName() << ".\n";
                }
                break;
            }
            case 3: { // Перевод
                senderUser = selectUser(Users, "Select sender for Transfer:");
                if (!senderUser) break;
                receiverUser = selectUser(Users, "Select receiver for Transfer:");
                if (!receiverUser || senderUser == receiverUser) {
                    std::cout << "Invalid receiver or cannot transfer to self.\n";
                    break;
                }
                amount = getValidatedInput<double>("Enter transfer amount: ");
                transactionSuccess = senderUser->MakeTransaction(receiverUser, amount, Action::TRANSFER);
                if (transactionSuccess) {
                    std::cout << "Transfer transaction added to pool from " << senderUser->GetName()
                              << " to " << receiverUser->GetName() << ".\n";
                }
                break;
            }
            case 4: { // Баланс Пользователей
                std::cout << "\n--- User Balances ---\n";
                // Burse выводим для дебага
                std::vector<User*> allParticipants = Users;
                allParticipants.insert(allParticipants.begin(), Burse);
                for (const auto& user : allParticipants) {
                    double balance = PGUCOIN->GetUserBalance(user);
                    std::cout << "User: " << user->GetName() << " (ID: " << user->GetID() << ") | Balance: "
                              << std::fixed << std::setprecision(2) << balance << std::endl;
                }
                break;
            }
            case 5: { // Весь блокчейн
                PGUCOIN->PrintChain();
                break;
            }
            case 6: {
                std::cout << "Exiting PGUCOIN. Goodbye!\n";
                break;
            }
            default: {
                std::cout << "Invalid choice. Please try again.\n";
                break;
            }
        }
    } while (choice != 6);

    delete PGUCOIN;
    for (User* user : Users) {
        delete user;
    }
    delete Burse;
    Users.clear();

    return 0;
}