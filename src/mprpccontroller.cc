#include "mprpccontroller.h"

std::string MpRpcController::ErrorText() const
{
    return err_text;
}
bool MpRpcController::Failed() const
{
    return m_failed;
}

void MpRpcController::Reset()
{
    err_text = "";
    m_failed = false;
}

void MpRpcController::SetFailed(const std::string &reason)
{
    err_text = reason;
    m_failed = true;
}
