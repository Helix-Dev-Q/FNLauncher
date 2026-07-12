#pragma once

#include "Config.h"

#include <string>

namespace FNLauncher {

bool startFortnite(
    const Build& build,
    const std::wstring& email,
    const std::wstring& password,
    unsigned long& pid,
    std::wstring& err);

}
