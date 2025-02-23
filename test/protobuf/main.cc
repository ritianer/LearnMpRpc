#include <iostream>
#include "test.pb.h"
using namespace fixbug;

int main(void)
{
    LoginRequest req;
    req.set_name("张三");
    req.set_pwd("123456");
    std::string send_str;
    send_str = req.SerializeAsString();
    std::cout<<send_str<<std::endl;
    LoginRequest req2;
    req2.ParseFromString(send_str);
    std::cout<<req2.pwd();
    return 0;
}