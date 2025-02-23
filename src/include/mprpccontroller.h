#pragma once
#include <google/protobuf/service.h>
class MpRpcController:public google::protobuf::RpcController
{
public:
    MpRpcController() : m_failed(false), err_text("") {}
    void Reset();
    bool Failed() const;
    void SetFailed(const std::string &reason);
    std::string ErrorText() const;
    // 未实现
    void StartCancel(){};
    bool IsCanceled()const { return 1; };
    void NotifyOnCancel(google::protobuf::Closure *callback){}; 

private:
    bool m_failed;
    std::string err_text;
};