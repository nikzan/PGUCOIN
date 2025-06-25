#include <iostream>
#include <iomanip>
#include <ctime>
#include <limits>
#include <vector>
#include <cstdio>
#include "../include/BlockChain.h"
#include "../include/AdditionalFunctions.h"
#include "../include/User.h"
#include "../include/Transaction.h"

std::vector<std::string> FirstNames;
std::vector<std::string> SecondNames;

void displayMenu() {
    std::cout << "\n--- PGUCOIN Menu ---\n";
    std::cout << "0. | Mine a Block | (Auto-saves)\n";
    std::cout << "1. Make a Deposit\n";
    std::cout << "2. Make a Withdrawal\n";
    std::cout << "3. Make a Transfer\n";
    std::cout << "4. View User Balances\n";
    std::cout << "5. View Blockchain\n";
    std::cout << "6. Exit\n";
    std::cout << "7. Clear Blockchain and Exit\n";
    std::cout << "Enter your choice: ";
}

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
    const std::string SAVE_FILE = "pgucoin.dat";

    FirstNames = ReadNames("../data/FirstNames.txt");
    SecondNames = ReadNames("../data/SecondNames.txt");
    if (FirstNames.empty() || SecondNames.empty()) {
        std::cerr << "Error: Could not load names...\n";
        return 1;
    }

    std::vector<User*> Users;

    BlockChain* PGUCOIN = new BlockChain("PGUCOIN_Network", SAVE_FILE, &Users);

    User* Burse = new User(0, "Burse", PGUCOIN);
    PGUCOIN->SetBurse(Burse);

    if (!PGUCOIN->loadFromFile()) {
        std::cout << "No saved blockchain found. Creating a new one.\n";

        int CountOfUsers = getValidatedInput<int>("Enter number of initial users (default = 3): ");
        if (CountOfUsers <= 0) CountOfUsers = 3;

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
    }

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
                    // Просто выход.
                    std::cout << "Exiting PGUCOIN. Goodbye!\n";
                    break;
            }
            case 7: {
                    if (remove(SAVE_FILE.c_str()) != 0) {
                        perror("Error deleting file");
                    } else {
                        std::cout << "Blockchain data file cleared.\n";
                    }
                    std::cout << "Exiting PGUCOIN. Goodbye!\n";
                    choice = 6;
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