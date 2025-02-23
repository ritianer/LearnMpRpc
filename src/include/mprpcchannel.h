#pragma once
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <google/protobuf/service.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <error.h>
#include "mprpcapplication.h"

class MpRpcChannel:public google::protobuf::RpcChannel
{
    public:
    //所有的   xxxx_stub 调用method 都使用这个函数实现
    //负责将rpc调用序列化并且通过网络发送
        void CallMethod(const google::protobuf::MethodDescriptor *method,
                        google::protobuf::RpcController *controller, const google::protobuf::Message *request,
                        google::protobuf::Message *response, google::protobuf::Closure *done);
};
