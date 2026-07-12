#pragma once

#include "Config.h"

#include <optional>
#include <string>

namespace FNLauncher {

bool validRoot(const std::wstring& root);
std::optional<Build> addBuild(const std::wstring& root, std::wstring& err);
std::optional<std::wstring> findExe(const std::wstring& root, std::wstring& err);

}
