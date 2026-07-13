#include "App.h"

#include "Backend.h"
#include "Builds.h"
#include "Console.h"
#include "Injector.h"
#include "Launcher.h"
#include "Paths.h"
#include "Process.h"

#include <algorithm>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace {

using namespace FNLauncher;

bool hasBuild(const Config& cfg, const std::wstring& root) {
    std::wstring target = norm(root);
    for (const Build& build : cfg.builds) {
        if (norm(build.root) == target) {
            return true;
        }
    }
    return false;
}

Build* getBuild(Config& cfg) {
    if (cfg.active < 0 || cfg.active >= (int)cfg.builds.size()) {
        return nullptr;
    }
    return &cfg.builds[cfg.active];
}

std::wstring getDll(const Config& cfg) {
    if (cfg.activeDll < 0 || cfg.activeDll >= (int)cfg.dlls.size()) {
        return {};
    }
    return cfg.dlls[cfg.activeDll];
}

void importBuild(Config& cfg) {
    clearScreen();
    printHeader();
    std::wcout << L"Import build\n\n";

    std::wstring root = pickFolder(L"Select Fortnite build folder");
    if (root.empty()) {
        std::wcout << L"Cancelled.\n";
        wait(2);
        return;
    }

    if (!validRoot(root)) {
        std::wcout << L"This folder does not contain FortniteGame and Engine.\n";
        wait(2);
        return;
    }

    if (hasBuild(cfg, root)) {
        for (size_t i = 0; i < cfg.builds.size(); i++) {
            if (norm(cfg.builds[i].root) == norm(root)) {
                cfg.active = (int)i;
                break;
            }
        }
        std::wcout << L"Build already imported.\n";
        wait(2);
        return;
    }

    std::wstring err;
    auto build = addBuild(root, err);
    if (!build) {
        std::wcout << err << L"\n";
        wait(2);
        return;
    }

    Build entry = *build;
    std::wcout << L"Name [" << entry.name << L"]: ";
    std::wcout.flush();

    std::wstring name;
    std::wcout.flush();
    name = readLine();
    if (!name.empty()) {
        entry.name = name;
    }

    cfg.builds.push_back(entry);
    cfg.active = (int)cfg.builds.size() - 1;
    std::wcout << L"Build imported.\n";
    wait(2);
}

void setRedirect(Config& cfg) {
    clearScreen();
    printHeader();
    std::wcout << L"Redirect DLL\n\n";

    std::wstring path = pickDll(L"Select redirect DLL");
    if (path.empty()) {
        std::wcout << L"Cancelled.\n";
        wait(2);
        return;
    }

    if (!exists(path)) {
        std::wcout << L"File not found.\n";
        wait(2);
        return;
    }

    std::wstring ext = fs::path(path).extension().wstring();
    if (_wcsicmp(ext.c_str(), L".dll") != 0) {
        std::wcout << L"Only DLL files are accepted.\n";
        wait(2);
        return;
    }

    cfg.dlls.clear();
    cfg.dlls.push_back(path);
    cfg.activeDll = 0;
    std::wcout << L"Redirect set.\n";
    wait(2);
}

void setConsole(Config& cfg) {
    clearScreen();
    printHeader();
    std::wcout << L"Console DLL\n\n";

    std::wstring path = pickDll(L"Select console DLL");
    if (path.empty()) {
        std::wcout << L"Cancelled.\n";
        wait(2);
        return;
    }

    if (!exists(path)) {
        std::wcout << L"File not found.\n";
        wait(2);
        return;
    }

    std::wstring ext = fs::path(path).extension().wstring();
    if (_wcsicmp(ext.c_str(), L".dll") != 0) {
        std::wcout << L"Only DLL files are accepted.\n";
        wait(2);
        return;
    }

    cfg.consoleDll = path;
    std::wcout << L"Console set.\n";
    wait(2);
}

void setGameServer(Config& cfg) {
    clearScreen();
    printHeader();
    std::wcout << L"Game Server DLL\n\n";

    std::wstring path = pickDll(L"Select game server DLL");
    if (path.empty()) {
        std::wcout << L"Cancelled.\n";
        wait(2);
        return;
    }

    if (!exists(path)) {
        std::wcout << L"File not found.\n";
        wait(2);
        return;
    }

    std::wstring ext = fs::path(path).extension().wstring();
    if (_wcsicmp(ext.c_str(), L".dll") != 0) {
        std::wcout << L"Only DLL files are accepted.\n";
        wait(2);
        return;
    }

    cfg.gameServerDll = path;
    std::wcout << L"Game server set.\n";
    wait(2);
}

void setPort(Config& cfg) {
    clearScreen();
    printHeader();
    std::wcout << L"Backend port (default 3551)\n\n";
    cfg.port = readPort(cfg.port);
    setBackendPort(cfg.port);
}

void setLogin(Config& cfg) {
    clearScreen();
    printHeader();
    std::wcout << L"Email and password\n\n";

    if (cfg.email.empty()) {
        std::wcout << L"Email: ";
    } else {
        std::wcout << L"Email [" << cfg.email << L", blank to keep]: ";
    }
    std::wcout.flush();
    std::wstring email = readLine();
    if (!email.empty()) {
        cfg.email = email;
    }

    if (cfg.password.empty()) {
        std::wcout << L"Password: ";
    } else {
        std::wcout << L"Password [" << cfg.password << L", blank to keep]: ";
    }
    std::wcout.flush();
    std::wstring pass = readLine();
    if (!pass.empty()) {
        cfg.password = pass;
    }

    std::wcout << L"Login saved.\n";
    wait(2);
}

void setClientLogin(Config& cfg) {
    clearScreen();
    printHeader();
    std::wcout << L"Client email and password\n\n";

    if (cfg.clientEmail.empty()) {
        std::wcout << L"Client email: ";
    } else {
        std::wcout << L"Client email [" << cfg.clientEmail << L", blank to keep]: ";
    }
    std::wcout.flush();
    std::wstring email = readLine();
    if (!email.empty()) {
        cfg.clientEmail = email;
    }

    if (cfg.clientPassword.empty()) {
        std::wcout << L"Client password: ";
    } else {
        std::wcout << L"Client password [" << cfg.clientPassword << L", blank to keep]: ";
    }
    std::wcout.flush();
    std::wstring pass = readLine();
    if (!pass.empty()) {
        cfg.clientPassword = pass;
    }

    std::wcout << L"Client login saved.\n";
    wait(2);
}

bool launchClient(
    Config& cfg,
    const std::wstring& email,
    const std::wstring& password,
    const wchar_t* label) {
    Build* build = getBuild(cfg);
    if (!build) {
        std::wcout << L"Import a build first.\n";
        wait(2);
        return false;
    }

    if (!validRoot(build->root)) {
        std::wcout << L"This folder does not contain FortniteGame and Engine.\n";
        wait(2);
        return false;
    }

    std::wstring dll = getDll(cfg);
    if (dll.empty()) {
        std::wcout << L"Set a redirect DLL first.\n";
        wait(2);
        return false;
    }

    std::wcout << L"Checking backend...\n";
    std::wcout.flush();

    if (!checkBackend()) {
        std::wcout << L"Backend isnt detected, if your backend is on you may have put the port wrong\n";
        std::wcout.flush();
        wait(3);
        return false;
    }

    if (!exists(build->exe)) {
        std::wstring err;
        auto exe = findExe(build->root, err);
        if (!exe) {
            std::wcout << err << L"\n";
            wait(2);
            return false;
        }
        build->exe = *exe;
    }

    unsigned long pid = 0;
    std::wstring err;
    bool firstClient = countFortnite() == 0;

    std::wcout << label << L"\n";
    std::wcout.flush();
    if (!startFortnite(*build, email, password, pid, err)) {
        std::wcout << err << L"\n";
        wait(2);
        return false;
    }

    std::wcout << L"Injecting redirect DLL...\n";
    std::wcout.flush();
    if (!injectDll(pid, dll, err)) {
        std::wcout << err << L"\n";
        wait(2);
        return false;
    }

    if (firstClient && !cfg.gameServerDll.empty()) {
        if (!exists(cfg.gameServerDll)) {
            std::wcout << L"Game server DLL not found.\n";
            wait(2);
            return false;
        }

        std::wcout << L"Injecting game server DLL...\n";
        std::wcout.flush();
        if (!injectDll(pid, cfg.gameServerDll, err)) {
            std::wcout << err << L"\n";
            wait(2);
            return false;
        }
    }

    if (!cfg.consoleDll.empty()) {
        std::wcout << L"Injecting console DLL...\n";
        std::wcout.flush();
        if (!injectDll(pid, cfg.consoleDll, err)) {
            std::wcout << err << L"\n";
            wait(2);
            return false;
        }
    }

    std::wcout << L"Launch complete.\n";
    wait(2);
    return true;
}

void doLaunch(Config& cfg) {
    clearScreen();
    printHeader();
    launchClient(
        cfg,
        effectiveEmail(cfg.email),
        effectivePassword(cfg.password),
        L"Starting Fortnite...");
}

void doLaunchAnotherClient(Config& cfg) {
    clearScreen();
    printHeader();

    launchClient(
        cfg,
        effectiveClientEmail(cfg.clientEmail),
        effectiveClientPassword(cfg.clientPassword),
        L"Starting another client...");
}

void doClose() {
    clearScreen();
    printHeader();

    std::wstring msg;
    closeFortnite(msg);
    std::wcout << msg << L"\n";
    wait(2);
}

}

namespace FNLauncher {

void run(Config& cfg) {
    setupConsole();
    setBackendPort(cfg.port);
    startBackend();

    while (true) {
        clearScreen();
        printHeader();
        printStatus(cfg, backendOnline());
        printMenu();

        int pick = readChoice(0, 10);
        if (pick == 0) {
            break;
        }

        switch (pick) {
        case 1: importBuild(cfg); break;
        case 2: setRedirect(cfg); break;
        case 3: setConsole(cfg); break;
        case 4: setGameServer(cfg); break;
        case 5: setPort(cfg); break;
        case 6: setLogin(cfg); break;
        case 7: doLaunch(cfg); break;
        case 8: setClientLogin(cfg); break;
        case 9: doLaunchAnotherClient(cfg); break;
        case 10: doClose(); break;
        }

        std::wstring err;
        saveConfig(cfg, err);
    }

    stopBackend();
}

}
