#include "mprpcchannel.h"
#include "rpcheader.pb.h"

void MpRpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                              google::protobuf::RpcController *controller, const google::protobuf::Message *request,
                              google::protobuf::Message *response, google::protobuf::Closure *done)
{
    // 序列化请求 并且打包
    std::string request_str = "";
    if (!request->SerializeToString(&request_str))
    {
        controller->SetFailed("request serialize error");
        return;
    }

    mprpc::RpcHeader header;
    header.set_service_name(method->service()->name());
    header.set_method_name(method->name());
    header.set_arg_size(request_str.length());
    std::string header_str = "";
    if (!header.SerializeToString(&header_str))
    {
        controller->SetFailed("header serialize error");
        return;
    }
    uint32_t header_length = header_str.length();

    std::string pack_str = ""; // header_length_str + header_str + request_str;
    pack_str.insert(0, std::string((char *)&header_length, 4));
    pack_str += header_str + request_str;
    std::cout << "package:" << pack_str << std::endl;
    // 网络传输部分
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (clientfd == -1)
    {
        controller->SetFailed("create socket error, errno:" + errno);
        return;
    }
    //从zookeeper服务器获取rpc服务的ip:port
    ZkClient zkCli;
    zkCli.start();
    auto addr_str = zkCli.GetData("/" + header.service_name() + "/" + header.method_name());
    int idx = addr_str.find_first_of(':');
    std::string ip = addr_str.substr(0, idx);
    uint16_t port = atoi(addr_str.substr(idx + 1, addr_str.length() - idx - 1).c_str());

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());
    if (-1 == connect(clientfd, (sockaddr *)&server_addr, sizeof(server_addr)))
    {
        controller->SetFailed("connect error, errno:" + errno);
        close(clientfd);
        return;
    }

    if (send(clientfd, pack_str.c_str(), pack_str.length(), 0) == -1)
    {
        controller->SetFailed("send error, errno:" + errno);
        close(clientfd);
        return;
    }
    char buffer[1024] = {0};
    int recv_size = recv(clientfd, buffer, 1024, 0);
    if (recv_size == -1)
    {
        controller->SetFailed("recv error, errno:" + errno);
        close(clientfd);
        return;
    }
    std::string response_str(buffer, 0, recv_size);
    if (!response->ParseFromString(response_str))
    {
        controller->SetFailed("parse error, response_str:" + response_str);
        close(clientfd);
        return;
    }
    close(clientfd);
    if(done)
        done->Run();
}