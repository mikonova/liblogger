#include "../src/liblogger.hpp"
#include <ostream>
#include <iostream>
#include <string>

int main() {
    std::string message = "";
    std::string levelString;
    int level;

    Logger log("log.txt", Level::LOG_MIN);
    while (true) {
        std::cout << "Введите сообщение и уровень важности" << std::endl;
        std::getline(std::cin, message);
        if (message == "exit" || message == "quit") break;
        std::getline(std::cin, levelString);
        if (levelString == "") {
            log.Log(message, Level::LOG_MIN);
            std::cout << "Сообщение: " << message << " внесено с уровнем важности " << Level::LOG_MIN << std::endl;
            continue;
        }
        try {
            level = std::stoi(levelString);
        } 
        catch (...)
        {
            std::cerr << "неверный формат уровня\n";
            continue;
        }
        if (level < 0 || level > 2) {
            std::cerr << "Уровень выходит за рамки нормы (0-2)\n";
            continue;
        }
        log.Log(message, level);
        std::cout << "Сообщение: " << message << " внесено с уровнем важности " << level << std::endl;

        
    }
}