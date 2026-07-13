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
    std::wstring clientEmail;
    std::wstring clientPassword;
    std::wstring consoleDll;
    std::wstring gameServerDll;
    int port = 3551;
    int active = -1;
    int activeDll = -1;
    std::vector<std::wstring> dlls;
    std::vector<Build> builds;
};

inline const wchar_t* defaultEmail() { return L"FN@FNLauncher.com"; }
inline const wchar_t* defaultPassword() { return L"FNLauncher"; }
inline const wchar_t* defaultClientEmail() { return L"Client@FNLauncher.com"; }
inline const wchar_t* defaultClientPassword() { return L"ClientLauncher"; }

std::wstring effectiveEmail(const std::wstring& email);
std::wstring effectivePassword(const std::wstring& password);
std::wstring effectiveClientEmail(const std::wstring& email);
std::wstring effectiveClientPassword(const std::wstring& password);

bool usingDefaultEmail(const std::wstring& email);
bool usingDefaultPassword(const std::wstring& password);
bool usingDefaultClientEmail(const std::wstring& email);
bool usingDefaultClientPassword(const std::wstring& password);

bool loadConfig(Config& cfg, std::wstring& err);
bool saveConfig(const Config& cfg, std::wstring& err);

}
