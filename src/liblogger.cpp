#include <ostream>
#include <sys/types.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include "liblogger.hpp"
#include <cstdlib>
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
#include <mutex>

using namespace std::chrono_literals;
using std::string;

Logger::Logger(string Path, int DefLoglevel) {
    #ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    #endif

    isLevelValid(DefLoglevel);
    isWorkerRunning = true;
    this->Path = Path;
    this->DefLoglevel = DefLoglevel;
    worker = std::thread(&Logger::socketWorkerLoop, this);
}

Logger::~Logger() {
    isWorkerRunning = 0;
    worker.join();
}

void Logger::Log(string message, int LogLevel) {
    if (message.empty()) {
        std::cout << "[WARN] liblogger: empty log message" << std::endl;
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
    std::string timeNow = getTimeNow();

    file << "[" << timeNow << "] " << "[level: " << LogLevel << "] " << message << std::endl;
}

void Logger::SocketLog(std::string message, int LogLevel) {
    if (message.empty()) {
        std::cout << "[WARN] liblogger: empty log message" << std::endl;
        return;
    }
    isLevelValid(LogLevel);
    if (LogLevel < DefLoglevel) {
        return;
    }
    std::string timeNow = getTimeNow();
    std::string msg = "[" + timeNow + "] " + "[level: " + std::to_string(LogLevel) + "] " + message + '\n';
    {
        std::lock_guard lock(chanMutex);
        chan.push(msg);
    }
}

void Logger::isLevelValid(int Level) {
    if (Level > 2 || Level < 0) {
        throw std::runtime_error("[ERROR] liblogger: loglevel out of range (0-2)");
    }
}

void Logger::socketWorkerLoop() {
    int sockWorker = -1;

    while (isWorkerRunning) {
        int res = 0;
        if (sockWorker == -1) {
            res = makeSock(&sockWorker);
        } 
        if (res == -1 ) continue;
        res = sendData(&sockWorker);
        if (res == -1) continue;
    }
}



int Logger::makeSock(int* sock) {
    *sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*sock == -1) return -1;

    struct sockaddr_in s;
    s.sin_family = AF_INET;
    s.sin_port = htons(Port);

    int err = inet_pton(AF_INET, IPAddress, &s.sin_addr);
    if (err == -1 || err == 0) {
        std::cerr << "[ERROR] liblogger: incorrect ip address" << '\n';
        exit(-1);
    }

    err = connect(*sock, (struct sockaddr *) &s, sizeof(s));
    if (err == -1) {
        std::cout << "[ERROR] liblogger: connection error, retrying in 5s" << std::endl;
        std::this_thread::sleep_for(5s);
        close(*sock);
        *sock = -1;
        return -1;
    }
    return 0;
}

int Logger::sendData(int* sock) {
    std::string msg;
    {
        std::lock_guard lock(chanMutex);
        msg = chan.front();
        chan.pop();
    }
    ssize_t sentCounter = 0;
    for (int byteCtr = msg.size(); byteCtr > sentCounter;) {
        ssize_t res = send(*sock, msg.data() + sentCounter, msg.size() - sentCounter, MSG_NOSIGNAL );
        if (res == -1) {
            *sock = -1;
            std::lock_guard lock(chanMutex);
            chan.push(msg);
            return res;
        }
        sentCounter += res;

    }
    return 0;
}

std::string Logger::getTimeNow() {
    auto timePoint = std::chrono::system_clock::now();
    time_t time = std::chrono::system_clock::to_time_t(timePoint);
    string formattedTime = (std::ctime(&time));
    formattedTime.pop_back();
    return formattedTime;
}