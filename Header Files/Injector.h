#pragma once

#include <string>

namespace FNLauncher {

bool injectDll(unsigned long pid, const std::wstring& dll, std::wstring& err);

}
