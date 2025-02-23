#pragma once

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <string>
#include <unordered_map>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <google/protobuf/service.h>

// 发布rpc服务的对象类
class RpcProvider
{
private:
public:
    //发布rpc方法
    void NotifyService(google::protobuf::Service *service);
    //启动rpc服务节点，提供rpc远程调用服务
    void Run();
private:
    std::unordered_map<std::string, google::protobuf::Service *> service_map;
    muduo::net::EventLoop m_eventLoop;
    void OnConnect(const muduo::net::TcpConnectionPtr &);
    void OnMessage(const muduo::net::TcpConnectionPtr &,
                   muduo::net::Buffer *,
                   muduo::Timestamp);
    void SendRpcResponse(const muduo::net::TcpConnectionPtr &, google::protobuf::Message *);
};
