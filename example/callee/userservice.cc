#include <iostream>
#include "user.pb.h"
#include "mprpcapplication.h"
using namespace fixbug;

//UserService业务，继承protobuf生成的类
class UserService:public UserServiceRpc
{
    public:
    //登陆功能代码
    bool Login(std::string name,std::string pwd)
    {
        std::cout<<"local service : Login name:"<<name<<" pwd:"<<pwd<<std::endl;
        return true;
    }
    //注册功能代码
    bool Register(uint32_t id, std::string name, std::string pwd)
    {
        std::cout << "local service : Register name:" << name << " pwd:" << pwd << " id:" << id << std::endl;
        return true;
    }
    //实现父类的虚函数
    void Login(google::protobuf::RpcController *controller,
               const fixbug::LoginRequest *request,
               fixbug::LoginResponse *response,
               google::protobuf::Closure *done)
    {
        bool res=Login(request->name(),request->pwd());
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("none");
        response->set_success(res);
        done->Run();
    }
    // 实现父类的虚函数
    void Register(google::protobuf::RpcController *controller,
                  const fixbug::RegisterRequest *request,
                  fixbug::RegisterResponse *response,
                  google::protobuf::Closure *done)
    {
        bool res = Register(request->id(), request->name(), request->pwd());
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("none");
        response->set_success(res);

        done->Run();
    }
};

int main(int argc,char** argv)
{
    //框架初始化
    MprpcApplication::init(argc,argv);
    // 把Service发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new UserService());
    //启动rpc节点,进程进入阻塞状态，等待远程rpc调用
    provider.Run();
    return 0;
}