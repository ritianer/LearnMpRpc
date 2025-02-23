#include <iostream>
#include "user.pb.h"
#include "mprpcapplication.h"


void login_complete(fixbug::LoginResponse &response)
{
    auto result = response.result();
    if (response.success())
        std::cout<< "登陆成功！" << std::endl;
    else
        std::cout << "登陆失败！ errcode:" << result.errcode() << " errrmsg:" << result.errmsg() << std::endl;
}
int main(int argc,char** argv)
{
    MprpcApplication::init(argc, argv);
    fixbug::UserServiceRpc_Stub stub(new MpRpcChannel());
    fixbug::LoginRequest request;
    request.set_name("zhansan");
    request.set_pwd("123456");
    fixbug::LoginResponse response;
    google::protobuf::Closure *callback = google::protobuf::NewCallback<fixbug::LoginResponse &>(&login_complete, response);
    stub.Login(nullptr, &request, &response, callback);

    fixbug::RegisterRequest request2;
    MpRpcController controller;
    request2.set_id(66);
    request2.set_name("lisi");
    request2.set_pwd("888888");
    fixbug::RegisterResponse response2;
    stub.Register(&controller, &request2, &response2, nullptr);
    if(controller.Failed())
    {
        std::cout << "Register failed. Error msg:" << controller.ErrorText() << std::endl;
    }
    else
    {
        auto result = response2.result();
        if (response2.success())
            std::cout
                << "注册成功！" << std::endl;
        else
            std::cout << "注册失败！ errcode:" << result.errcode() << " errrmsg:" << result.errmsg() << std::endl;
    }
    

    return 0;
}

