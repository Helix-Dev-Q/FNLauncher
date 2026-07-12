#include "Backend.h"
#include "Server.h"

#include <Windows.h>

#include <atomic>

namespace {

std::atomic<int> port{3551};
std::atomic<bool> online{false};
std::atomic<bool> alive{false};
HANDLE thread = nullptr;

void refresh() {
    online.store(FNLauncher::pingBackend(L"127.0.0.1", port.load()));
}

DWORD WINAPI loop(LPVOID) {
    while (alive.load()) {
        refresh();
        Sleep(2000);
    }
    return 0;
}

}

namespace FNLauncher {

void setBackendPort(int value) {
    port.store(value);
    refresh();
}

void startBackend() {
    if (thread) {
        return;
    }
    alive.store(true);
    thread = CreateThread(nullptr, 0, loop, nullptr, 0, nullptr);
}

void stopBackend() {
    if (!thread) {
        return;
    }
    alive.store(false);
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
    thread = nullptr;
}

bool checkBackend() {
    refresh();
    return online.load();
}

bool backendOnline() {
    return online.load();
}

}
