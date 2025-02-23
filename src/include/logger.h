#pragma once
#include "lockquene.h"
#include <string>
#include <string.h>
#include <iostream>

enum LogLevel
{
    INFO,
    ERROR
};
class Logger
{
public:
    static Logger &GetInstance();
    void Log(std::string msg, LogLevel = INFO);

private:
    LockQuene<std::pair<LogLevel, std::string>> m_quene;
    Logger();
    Logger(const Logger &) = delete;
    Logger(Logger &&) = delete;
};

#define LOG_INFO(logmsgformat, ...)              \
    do                                           \
    {                                            \
        Logger &logger = Logger::GetInstance();  \
        char c[1024] = {0};                      \
        sprintf(c, logmsgformat, ##__VA_ARGS__); \
        logger.Log(c);                           \
    } while (0);

#define LOG_ERR(logmsgformat, ...)               \
    do                                           \
    {                                            \
        Logger &logger = Logger::GetInstance();  \
        char c[1024] = {0};                      \
        sprintf(c, logmsgformat, ##__VA_ARGS__); \
        logger.Log(c, ERROR);                    \
    } while (0);
