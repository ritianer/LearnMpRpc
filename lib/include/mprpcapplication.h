#pragma once

#include "maprpcconfig.h"
#include "mprpcchannel.h"
#include "rpcprovider.h"
#include "mprpccontroller.h"
#include "logger.h"
#include "zookeeperutil.h"
//mprpc框架的基础类，负责初始化框架
class MprpcApplication
{
public:
    static void init(int argc, char **argv);
    static MprpcApplication& GetInstance();
    static MprpcConfig& GetConfig();

private:
    static MprpcConfig m_config;
    MprpcApplication(){}
    MprpcApplication(const MprpcApplication &) = delete;
    MprpcApplication(MprpcApplication &&) = delete;
};