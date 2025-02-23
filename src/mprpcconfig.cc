#include "maprpcconfig.h"
#include <iostream>
void MprpcConfig::LoadConfigFile(const char *config_file)
{
    FILE *f = fopen(config_file, "r");
    if (!f)
    {
        std::cout << config_file << " is nor exsit" << std::endl;
        exit(EXIT_FAILURE);
    }
    while (!feof(f))
    {
        char buf[512] = {0};
        fgets(buf, 512, f);
        int l = 0;
        for (int i = 0; i < 512; i++)
        {
            if (buf[i] == '\0')
                break;
            if (buf[i] != ' ' && buf[i] != '\n' && buf[i] != '\t')
            {
                if (l != i)
                    buf[l] = buf[i];
                l++;
            }
        }
        buf[l] = '\0';
        std::string src_buf(buf);
        if (src_buf[0] == '#' || src_buf.empty())
            continue;

        int idx = src_buf.find('=');
        if (idx == -1)
            continue;
        std::string key = src_buf.substr(0, idx);
        std::string value = src_buf.substr(idx + 1, src_buf.size() - idx);
        m_configmap.insert({key, value});
    }
}
std::string MprpcConfig::Load(const std::string &key)
{
    auto it = m_configmap.find(key);
    if (it != m_configmap.end())
    {
        return it->second;
    }
    return "";
}