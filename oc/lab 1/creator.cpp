//cd "/Users/olga_zalutskaya/Documents/oc/lab 1/"
//./creator employees.dat 3

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <limits>
#include <cctype>
#include <cstdlib>
#include "employee.h"

// Функция для проверки выхода
bool shouldExit(const std::string& input) {
    return input == "exit" || input == "quit" || input == "выход";
}

// Функция для безопасного ввода целого числа с возможностью выхода
bool safeInputInt(const std::string& prompt, int& value) {
    std::string input;
    while (true) {
        std::cout << prompt << " (или 'exit' для выхода): ";
        std::getline(std::cin, input);
        
        if (shouldExit(input)) {
            return false; // Выход
        }
        
        if (input.empty()) {
            std::cout << "Ошибка! Введите число или 'exit'." << std::endl;
            continue;
        }
        
        // Проверяем, что все символы - цифры или минус
        bool valid = true;
        for (size_t i = 0; i < input.length(); i++) {
            if (i == 0 && input[i] == '-') {
                continue; // Минус в начале допустим
            }
            if (!std::isdigit(input[i])) {
                valid = false;
                break;
            }
        }
        
        if (!valid) {
            std::cout << "Ошибка! Введите целое число или 'exit'." << std::endl;
            continue;
        }
        
        value = std::atoi(input.c_str());
        return true; // Успешный ввод
    }
}

// Функция для безопасного ввода числа с плавающей точкой с возможностью выхода
bool safeInputDouble(const std::string& prompt, double& value) {
    std::string input;
    while (true) {
        std::cout << prompt << " (или 'exit' для выхода): ";
        std::getline(std::cin, input);
        
        if (shouldExit(input)) {
            return false; // Выход
        }
        
        if (input.empty()) {
            std::cout << "Ошибка! Введите число или 'exit'." << std::endl;
            continue;
        }
        
        // Проверяем, что все символы - цифры, точка, или минус
        bool valid = true;
        bool hasDot = false;
        for (size_t i = 0; i < input.length(); i++) {
            if (i == 0 && input[i] == '-') {
                continue; // Минус в начале допустим
            }
            if (input[i] == '.' && !hasDot) {
                hasDot = true;
                continue; // Одна точка допустима
            }
            if (!std::isdigit(input[i])) {
                valid = false;
                break;
            }
        }
        
        if (!valid) {
            std::cout << "Ошибка! Введите число или 'exit'." << std::endl;
            continue;
        }
        
        value = std::atof(input.c_str());
        return true; // Успешный ввод
    }
}

// Функция для безопасного ввода имени (только буквы) с возможностью выхода
bool safeInputName(char* nameBuffer, int bufferSize) {
    std::string input;
    while (true) {
        std::cout << "Имя (макс. " << (bufferSize - 1) << " символов, только буквы, или 'exit' для выхода): ";
        std::getline(std::cin, input);
        
        if (shouldExit(input)) {
            return false; // Выход
        }
        
        // Проверяем, что строка не пустая
        if (input.empty()) {
            std::cout << "Ошибка! Имя не может быть пустым." << std::endl;
            continue;
        }
        
        // Проверяем, что все символы - буквы
        bool valid = true;
        for (char c : input) {
            if (!std::isalpha(c)) {
                valid = false;
                break;
            }
        }
        
        if (!valid) {
            std::cout << "Ошибка! Имя должно содержать только буквы." << std::endl;
            continue;
        }
        
        // Проверяем длину
        if (input.length() >= bufferSize) {
            std::cout << "Ошибка! Имя слишком длинное. Максимум " << (bufferSize - 1) << " символов." << std::endl;
            continue;
        }
        
        // Копируем в буфер
        std::strncpy(nameBuffer, input.c_str(), bufferSize - 1);
        nameBuffer[bufferSize - 1] = '\0'; // Гарантируем нулевое завершение
        return true; // Успешный ввод
    }
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cout << "Использование: Creator <filename> <record_count>" << std::endl;
        return 1;
    }
    
    const char* filename = argv[1];
    int recordCount = std::atoi(argv[2]);
    
    if (recordCount <= 0)
    {
        std::cout << "Количество записей должно быть положительным числом" << std::endl;
        return 1;
    }
    
    // Создаем файл
    FILE* file = fopen(filename, "wb");
    if (!file)
    {
        std::cout << "Ошибка создания файла" << std::endl;
        return 1;
    }
    
    std::cout << "Введите данные " << recordCount << " сотрудников:" << std::endl;
    std::cout << "В любой момент можно ввести 'exit' для выхода" << std::endl;
    
    for (int i = 0; i < recordCount; i++)
    {
        employee emp = {0}; // Инициализируем нулями
        
        std::cout << "\n--- Сотрудник " << (i + 1) << " ---" << std::endl;
        
        // Безопасный ввод ID номера с возможностью выхода
        int num;
        if (!safeInputInt("ID номер: ", num)) {
            std::cout << "Выход из программы..." << std::endl;
            fclose(file);
            return 0;
        }
        emp.num = num;
        
        // Безопасный ввод имени с возможностью выхода
        if (!safeInputName(emp.name, sizeof(emp.name))) {
            std::cout << "Выход из программы..." << std::endl;
            fclose(file);
            return 0;
        }
        
        // Безопасный ввод отработанных часов с возможностью выхода
        double hours;
        if (!safeInputDouble("Отработанные часы: ", hours)) {
            std::cout << "Выход из программы..." << std::endl;
            fclose(file);
            return 0;
        }
        emp.hours = hours;
        
        // Записываем в файл
        fwrite(&emp, sizeof(employee), 1, file);
        
        std::cout << "✓ Данные сотрудника " << (i + 1) << " сохранены." << std::endl;
    }
    
    fclose(file);
    std::cout << "\n✓ Файл " << filename << " успешно создан с " << recordCount << " записями." << std::endl;
    
    return 0;
}