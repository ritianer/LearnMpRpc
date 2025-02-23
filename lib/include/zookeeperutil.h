#pragma once
#define THREADED
#include <semaphore.h>
#include <zookeeper/zookeeper.h>
#include <string>
#include "mprpcapplication.h"
class ZkClient
{
public:
    ZkClient();
    ~ZkClient();
    void start();
    void create(const std::string &path,const std::string &data, int state = 0);
    std::string GetData(const std::string &path);

private:
    zhandle_t *m_zhandle;
};