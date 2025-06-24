// AdditionalFunctions.h
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "User.h"

std::vector<std::string> ReadNames(const std::string& FileName) {

    std::ifstream File(FileName);

    if (!File.is_open()) {
        std::cerr << "Ошибка: Не удалось открыть файл: " << FileName << std::endl;
        return {};
    }

    std::vector<std::string> names;
    std::string Input;
    while (File >> Input) {
        names.push_back(Input);
    }

    File.close();
    return names;
}


std::string GenerationName(const std::vector<std::string>& FirstNames, const std::vector<std::string>& SecondNames) {

    if (FirstNames.empty() || SecondNames.empty()) {
        return "Unknown User"; // Добавил случай, если векторы пусты
    }

    int FistEnd = FirstNames.size() - 1;
    int SecondEnd = SecondNames.size() - 1;

    int FisrtNameInd = rand() % (FistEnd + 1);
    int SecondNameInd = rand() % (SecondEnd + 1);

    return FirstNames[FisrtNameInd] + " " + SecondNames[SecondNameInd];
}

bool CheckUserName(const std::vector<User*>& Users, const std::string& Name) {
    for (int i = 0; i < Users.size(); i++){
        if (Users[i]->GetName() == Name)
            return false;
    }
    return true;
}