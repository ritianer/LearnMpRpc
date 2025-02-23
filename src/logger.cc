#include "logger.h"

Logger &Logger::GetInstance()
{
    static Logger instance;
    return instance;
}


void Logger::Log(std::string msg,LogLevel level)
{
    std::pair<LogLevel, std::string> p({level, msg});
    m_quene.Push(p);
}

Logger::Logger()
{
    
    std::thread write_logger_thread([&]()
                                    {
        while (true)
        {
            time_t now = time(nullptr);
            tm *nowtm = localtime(&now);
            char filename[128];
            sprintf(filename, "%d-%d-%d-log.txt", nowtm->tm_year + 1900, nowtm->tm_mon + 1, nowtm->tm_mday);
            //打开日志文件
            FILE *f = fopen(filename, "a+");
            if (!f)
            {
                std::cout << "logger file:" << filename << " open error." << std::endl;
                exit(EXIT_FAILURE);
            }
            //写入日志
            auto p = m_quene.Pop();

            std::string loglv_str = "[ERR]";
            if(p.first == INFO)
                loglv_str = "[INFO]";

            std::string msg = p.second;
            char timebuf[128] = "";
            sprintf(timebuf, "[%d:%d:%d]", nowtm->tm_hour, nowtm->tm_min, nowtm->tm_sec);
            msg.insert(0, timebuf);
            msg = loglv_str + msg + "\n";
            fputs(msg.c_str(), f);
            fclose(f);
        } });
    write_logger_thread.detach();
}