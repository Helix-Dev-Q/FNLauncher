#pragma once

#include <string>
#include <vector>

namespace FNLauncher {

struct Build {
    std::wstring name;
    std::wstring root;
    std::wstring exe;
};

struct Config {
    std::wstring email;
    std::wstring password;
    std::wstring consoleDll;
    std::wstring gameServerDll;
    int port = 3551;
    int active = -1;
    int activeDll = -1;
    std::vector<std::wstring> dlls;
    std::vector<Build> builds;
};

bool loadConfig(Config& cfg, std::wstring& err);
bool saveConfig(const Config& cfg, std::wstring& err);

}
