#include "App.h"
#include "Config.h"
#include "Console.h"

#include <iostream>

using namespace FNLauncher;

int wmain() {
    Config cfg;
    std::wstring err;

    if (!loadConfig(cfg, err)) {
        setupConsole();
        std::wcout << L"Failed to load settings.\n";
        return 1;
    }

    run(cfg);

    if (!saveConfig(cfg, err) && !err.empty()) {
        setupConsole();
        std::wcout << err << L"\n";
        return 1;
    }

    return 0;
}
