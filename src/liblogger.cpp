#ifdef _WIN32
#include <windows.h>
#endif
#include "liblogger.hpp"
#include <iostream>
#include <ios>
#include <string>
#include <chrono>
#include <fstream>
#include <ctime>
#include <stdexcept>
#include <queue>
#include <thread>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std::chrono_literals;
using std::string;

Logger::Logger(string Path, int DefLoglevel) {
    #ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    #endif

    isLevelValid(DefLoglevel);
    this->Path = Path;
    this->DefLoglevel = DefLoglevel;
}

void Logger::Log(string message, int LogLevel) {
    if (message.empty()) {
        std::cout << "[WARN] liblogger: empty log message" << "\n";
        return;
    }
    isLevelValid(LogLevel);
    if (LogLevel < DefLoglevel) {
        return;
    }

    std::fstream file(Path, std::ios::app | std::ios::in);
    if (!file.is_open()) {
        throw std::runtime_error("[ERROR] liblogger: unable to open the file");
    }
    auto timePoint = std::chrono::system_clock::now();
    time_t time = std::chrono::system_clock::to_time_t(timePoint);
    string formattedTime = (std::ctime(&time));
    formattedTime.pop_back();

    file << "[" << formattedTime << "] " << "[level: " << LogLevel << "] " << message << std::endl;
}

void Logger::isLevelValid(int Level) {
    if (Level > 2 || Level < 0) {
        throw std::runtime_error("[ERROR] liblogger: loglevel out of range (0-2)");
    }
}

void Logger::socketWorkerLoop() {
    int sockWorker = -1;

    while (true) {
        makeSock(&sockWorker);

    }
}

int Logger::makeSock(int* sock) {
    if (*sock != -1) {
        return -1;
    }
    *sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in s;
    s.sin_family = AF_INET;
    s.sin_port = htons(Port);

    int err = inet_pton(AF_INET, IPAddress, &s.sin_addr);
    if (err == -1) {
        std::cerr << "[ERROR] liblogger: incorrect ip address" << '\n';
    }

    err = connect(*sock, (struct sockaddr *) &s, sizeof(s));
    if (err != -1) {
        std::cout << "[ERROR] liblogger: connection error, retrying in 5s" << '\n';
        std::this_thread::sleep_for(5s);
        close(*sock);
        *sock = -1;
    }  
}

void Logger::sendData(int* sock) {

}