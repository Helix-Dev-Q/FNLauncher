#include "Injector.h"

#include <Windows.h>

namespace FNLauncher {

bool injectDll(unsigned long pid, const std::wstring& dll, std::wstring& err) {
    HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
    if (!process) {
        err = L"Could not open Fortnite process.";
        return false;
    }

    size_t size = (dll.size() + 1) * sizeof(wchar_t);
    void* remote = VirtualAllocEx(process, nullptr, size, 0x3000, 0x04);
    if (!remote) {
        CloseHandle(process);
        err = L"Injection failed.";
        return false;
    }

    if (!WriteProcessMemory(process, remote, dll.c_str(), size, nullptr)) {
        VirtualFreeEx(process, remote, 0, 0x8000);
        CloseHandle(process);
        err = L"Injection failed.";
        return false;
    }

    auto load = (LPTHREAD_START_ROUTINE)GetProcAddress(
        GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW");
    if (!load) {
        VirtualFreeEx(process, remote, 0, 0x8000);
        CloseHandle(process);
        err = L"Injection failed.";
        return false;
    }

    HANDLE worker = CreateRemoteThread(process, nullptr, 0, load, remote, 0, nullptr);
    if (!worker) {
        VirtualFreeEx(process, remote, 0, 0x8000);
        CloseHandle(process);
        err = L"Injection failed.";
        return false;
    }

    WaitForSingleObject(worker, 10000);
    CloseHandle(worker);
    VirtualFreeEx(process, remote, 0, 0x8000);
    CloseHandle(process);
    return true;
}

}
