#pragma once

#include <string>
#include <vector>

namespace FNLauncher {

std::wstring norm(const std::wstring& path);
bool exists(const std::wstring& path);
bool isDir(const std::wstring& path);

std::wstring join(const std::wstring& left, const std::wstring& right);
std::wstring baseName(const std::wstring& path);
std::wstring trim(const std::wstring& text);

std::wstring configFile();
std::wstring legacyConfigFile();

std::wstring pickFolder(const std::wstring& title);
std::wstring pickDll(const std::wstring& title);

std::vector<std::wstring> findFile(
    const std::wstring& root,
    const std::wstring& name,
    int depth);

}
