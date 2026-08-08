#pragma once
#include <atomic>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <condition_variable>

enum Level {
        LOG_MIN = 0,
        LOG_MED,
        LOG_MAX
};

// implements a simple logger, which writes to path with the Loglevel between 0 (lowest) and 2 (highest)
class Logger {
    public:
    int DefLoglevel;
    int Port = 44044;
    char IPAddress[15];
    std::string Path;
    
    // initialize a new logger instance and start the worker loop
    Logger(std::string Path, int DefLoglevel);
    ~Logger();

    // writes message to the log file
    void Log(std::string message, int LogLevel);

    // writes message to the socket and sends it to ip defined with IPAdress and Port
    void SocketLog(std::string message, int LogLevel);

    private:

    // checks level validity
    void isLevelValid(int Level);

    //starts the loop of sending messages
    void socketWorkerLoop();

    // creates a new socket handle
    int makeSock(int* sock);

    // sends data via socket
    int sendData(int* sock);

    // gets time current time
    std::string getTimeNow();

    std::thread worker;
    std::mutex chanMutex;
    std::mutex fileMutex;
    std::queue<std::string> chan;
    std::atomic<bool> isWorkerRunning = false;
    std::condition_variable cv;
    
};
