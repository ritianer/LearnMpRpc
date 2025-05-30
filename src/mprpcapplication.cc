#include "mprpcapplication.h"
#include <iostream>
#include <unistd.h>
#include <string>

MprpcConfig MprpcApplication::m_config;

void ShowArgsHelp()
{
    std::cout<<"format: command -i <configfile>"<<std::endl;
}

MprpcApplication&  MprpcApplication::GetInstance()
{
    static  MprpcApplication instance;
    return instance;
}

void MprpcApplication::init(int argc,char** argv)
{
    if(argc<2)
    {
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }

    int c=0;
    std::string config_file;
    while((c=getopt(argc,argv,"i:"))!=-1)//获取输入参数
    {
        switch(c)
        {
            case 'i':
            config_file=optarg;
            break;
            case '?':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
            case ':':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
            default:
            break;
        }
    }
    m_config.LoadConfigFile(config_file.c_str());//加载配置文件
    std::cout << "rpcserverip:" << m_config.Load("rpcserverip") << std::endl;
    std::cout << "rpcserverport:" << m_config.Load("rpcserverport") << std::endl;
    std::cout << "zookeeperip:" << m_config.Load("zookeeperip") << std::endl;
    std::cout << "zookeeperport:" << m_config.Load("zookeeperport") << std::endl;
}

MprpcConfig &MprpcApplication::GetConfig()
{
    return m_config;
}