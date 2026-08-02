#include <mutex>
#include <queue>
#include <string>
#include <thread>

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

    Logger(std::string Path, int DefLoglevel);
    ~Logger();

    void Log(std::string message, int LogLevel);
    void SocketLog(std::string message, int LogLevel);

    private:
    void isLevelValid(int Level);
    void socketWorkerLoop();
    int makeSock(int* sock);
    int sendData(int* sock);
    std::string getTimeNow();
    std::thread worker;
    std::mutex chanMutex;
    std::queue<std::string> chan;
    bool isWorkerRunning = false;
    
};
