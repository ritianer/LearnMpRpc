#include "rpcprovider.h"
#include "mprpcapplication.h"
#include "rpcheader.pb.h"

void RpcProvider::NotifyService(google::protobuf::Service *service)
{
    service_map.insert({service->GetDescriptor()->name(), service});
    std::cout << "NotifyService:" << service->GetDescriptor()->name() << std::endl;
}

void RpcProvider::Run()
{
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip, port);
    // 创建tpc server
    muduo::net::TcpServer Server(&m_eventLoop, address, "RpcProvider");
    // 绑定连接回调和消息读写回调方法
    Server.setConnectionCallback(std::bind(&RpcProvider::OnConnect, this, std::placeholders::_1));
    Server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    // 设置muduo库的线程数量
    Server.setThreadNum(4);

    //将rpc节点要发布的服务全部注册到zk上，让rpc_client可以从zk发现服务
    ZkClient zkCli;
    zkCli.start();
    //service_name 为永久节点，method_name为临时节点
    for(auto &sp:service_map)
    {
        std::string service_path = "/" + sp.first;
        zkCli.create(service_path, "");
        auto desc = sp.second->GetDescriptor();
        for (int i = 0; i < desc->method_count();i++)
        {
            std::string method_path = service_path + "/" + desc->method(i)->name();
            std::string method_data = ip + ":" + std::to_string(port);
            zkCli.create(method_path, method_data, ZOO_EPHEMERAL);
        }
    }
    // 启动网络服务
    std::cout << "RpcProvider start at ip: " << ip << " port: " << port << std::endl;
    Server.start();
    m_eventLoop.loop();
}

void RpcProvider::OnConnect(const muduo::net::TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {
        conn->shutdown();
    }
}

void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr &conn,
                            muduo::net::Buffer *buffer,
                            muduo::Timestamp)
{
    std::string buf = buffer->retrieveAllAsString();
    for (int i = 0; i < 5;i++)
    {
        std::cout << "char:" << buf[i] << " byte:" << (uint8_t)buf[i] << std::endl;
    }
        // 包结构：header_size(int32) + 包头（RpcHeader）+请求（Message）
        uint32_t header_size = 0;
    buf.copy((char *)&header_size, 4, 0);
    std::string rpcheader_str = buf.substr(4, header_size);
    mprpc::RpcHeader header;
    std::string service_name, method_name, arg_str;
    uint32_t arg_size;
    // 解包
    if (header.ParseFromString(rpcheader_str))
    {
        service_name = header.service_name();
        method_name = header.method_name();
        arg_size = header.arg_size();
    }
    else
    {
        std::cout << "RpcHeader:" << rpcheader_str << "  ParseError!" << std::endl;
        return;
    }
    arg_str = buf.substr(4 + header_size, arg_size);
    std::cout << "===================================================" << std::endl;
    std::cout << "service name:" << service_name << std::endl;
    std::cout << "method name:" << method_name << std::endl;
    std::cout << "arg size:" << arg_size << std::endl;
    std::cout << "arg string:" << arg_str << std::endl;
    std::cout << "===================================================" << std::endl;

    // 调用具体函数
    auto it = service_map.find(service_name);
    if (it == service_map.end())
    {
        std::cout << "Service:" << service_name << " is not exist." << std::endl;
        return;
    }
    auto method_desc = it->second->GetDescriptor()->FindMethodByName(method_name);
    if (!method_desc)
    {
        std::cout << "Method:" << method_name << " is not exist." << std::endl;
        return;
    }
    google::protobuf::Message *request = it->second->GetRequestPrototype(method_desc).New();
    if(!request->ParseFromString(arg_str))
    {
        std::cout << "request parse error,content:" << arg_str << std::endl;
        return;
    }
    google::protobuf::Message *response = it->second->GetResponsePrototype(method_desc).New();
    google::protobuf::Closure *callback = google::protobuf::NewCallback<RpcProvider, const muduo::net::TcpConnectionPtr &, google::protobuf::Message *>(this, &RpcProvider::SendRpcResponse, conn, response);
    it->second->CallMethod(method_desc, nullptr, request, response, callback);
}

void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr &conn, google::protobuf::Message *response)
{
    std::string response_str;
    if(response->SerializeToString(&response_str))
    {
        std::cout << "send to client:" << response_str << std::endl;
        conn->send(response_str);
    }
    else
    {
        std::cout << "request serialize error,content:" << response_str << std::endl;
    }
    conn->shutdown();
}
