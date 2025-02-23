#include "zookeeperutil.h"

ZkClient::ZkClient():m_zhandle(nullptr){}

ZkClient::~ZkClient()
{
    if(!m_zhandle)
        zookeeper_close(m_zhandle);
}

void global_watcher(zhandle_t* zh,int eventType,int state,const char *path,void *watcherCtx)
{
    if (eventType == ZOO_SESSION_EVENT)
    {
        if(state==ZOO_CONNECTED_STATE)
        {
            sem_t *sem = (sem_t *)zoo_get_context(zh);
            sem_post(sem);
        }
    }
}

void ZkClient::start()
{
    std::string host = MprpcApplication::GetInstance().GetConfig().Load("zookeeperip");
    std::string port = MprpcApplication::GetInstance().GetConfig().Load("zookeeperport");
    auto connstr = host + ":" + port;
    m_zhandle = zookeeper_init(connstr.c_str(), global_watcher, 30000, nullptr, nullptr, 0);
    if(m_zhandle==nullptr)
    {
        std::cout << "zookeeper init error." << std::endl;
        exit(EXIT_FAILURE);
    }
    sem_t sem;
    sem_init(&sem, 0, 0);
    zoo_set_context(m_zhandle, &sem);
    sem_wait(&sem);
    std::cout << "zookeeper init success." << std::endl;
}

void ZkClient::create(const std::string &path,const std::string &data, int state)
{
    char pathbuffer[128];
    int bufferlen = sizeof(pathbuffer);
    int flag = zoo_exists(m_zhandle, path.c_str(), 0, nullptr);
    if(flag!=ZNONODE)
        return;
    flag = zoo_create(m_zhandle, path.c_str(), data.c_str(), data.length(), &ZOO_OPEN_ACL_UNSAFE, state, pathbuffer, bufferlen);
    if (flag == ZOK)
    {
        std::cout << "znode create success,path:" << path << std::endl;
    }
    else
    {
        std::cout << "flag:" << std::endl;
        std::cout << "znode create success,path:" << path << std::endl;
        exit(EXIT_FAILURE);
    }
}

std::string ZkClient::GetData(const std::string &path)
{
    char buffer[64];
    int bufflen = sizeof(buffer);
    int flag = zoo_get(m_zhandle, path.c_str(), 0, buffer, &bufflen, nullptr);
    if (flag != ZOK)
    {
        std::cout << "flag:" << std::endl;
        std::cout << "get znode error,path:" << path << std::endl;
        return "";
    }
    std::string res(buffer);
    return res;
}