#include "Console.h"

#include <Windows.h>

#include <filesystem>
#include <iostream>
#include <limits>

namespace fs = std::filesystem;

namespace {

std::wstring trimLine(const std::wstring& text) {
    size_t start = text.find_first_not_of(L" \t\r\n");
    if (start == std::wstring::npos) {
        return {};
    }
    size_t end = text.find_last_not_of(L" \t\r\n");
    return text.substr(start, end - start + 1);
}

std::wstring maskPassword(const std::wstring& pass) {
    if (pass.empty()) {
        return L"none";
    }
    return std::wstring(pass.size(), L'*');
}

}

namespace FNLauncher {

void setupConsole() {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    std::ios::sync_with_stdio(false);
    std::wcin.tie(&std::wcout);
}

void clearScreen() {
    system("cls");
}

void printHeader() {
    std::wcout << L"FNLauncher made by helix.67 on discord!\n";
    std::wcout << L"---------\n\n";
}

void printStatus(const Config& cfg, bool backendUp) {
    std::wcout << L"Email:    " << (cfg.email.empty() ? L"none" : cfg.email) << L"\n";
    std::wcout << L"Password: " << maskPassword(cfg.password) << L"\n";
    std::wcout << L"Port:     " << cfg.port << L"\n";
    std::wcout << L"Backend:  " << (backendUp ? L"online" : L"offline") << L"\n";

    if (cfg.builds.empty() || cfg.active < 0 || cfg.active >= (int)cfg.builds.size()) {
        std::wcout << L"Build:    none\n";
    } else {
        std::wcout << L"Build:    " << cfg.builds[cfg.active].name << L"\n";
    }

    if (cfg.dlls.empty() || cfg.activeDll < 0 || cfg.activeDll >= (int)cfg.dlls.size()) {
        std::wcout << L"Redirect/Authentication: none\n";
    } else {
        std::wcout << L"Redirect/Authentication: " << fs::path(cfg.dlls[cfg.activeDll]).filename().wstring() << L"\n";
    }

    if (cfg.consoleDll.empty()) {
        std::wcout << L"Console: none\n";
    } else {
        std::wcout << L"Console: " << fs::path(cfg.consoleDll).filename().wstring() << L"\n";
    }

    if (cfg.gameServerDll.empty()) {
        std::wcout << L"Game Server: none\n";
    } else {
        std::wcout << L"Game Server: " << fs::path(cfg.gameServerDll).filename().wstring() << L"\n";
    }

    std::wcout << L"\n";
}

void printMenu() {
    std::wcout << L"1. Import build\n";
    std::wcout << L"2. Redirect/Authentication\n";
    std::wcout << L"3. Console\n";
    std::wcout << L"4. Game Server\n";
    std::wcout << L"5. Backend port\n";
    std::wcout << L"6. Email and Password\n";
    std::wcout << L"7. Launch\n";
    std::wcout << L"8. Close Fortnite\n";
    std::wcout << L"0. Exit\n\n";
    std::wcout << L"Choice: ";
}

int readChoice(int low, int high) {
    while (true) {
        std::wstring line = readLine();
        if (line.empty()) {
            std::wcout << L"Enter " << low << L"-" << high << L": ";
            continue;
        }

        try {
            int value = std::stoi(line);
            if (value >= low && value <= high) {
                return value;
            }
        } catch (...) {
        }

        std::wcout << L"Enter " << low << L"-" << high << L": ";
    }
}

std::wstring readLine() {
    std::wstring line;
    std::getline(std::wcin, line);
    return trimLine(line);
}

int readPort(int current) {
    std::wcout << L"Backend port [" << current << L"]: ";
    std::wcout.flush();

    std::wstring line = readLine();
    if (line.empty()) {
        return current;
    }

    try {
        int value = std::stoi(line);
        if (value >= 1 && value <= 65535) {
            return value;
        }
    } catch (...) {
    }

    std::wcout << L"Invalid port, keeping " << current << L"\n";
    return current;
}

void wait(int seconds) {
    if (seconds > 0) {
        Sleep(seconds * 1000);
    }
}

}
