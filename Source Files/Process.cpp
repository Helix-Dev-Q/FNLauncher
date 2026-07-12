#include "Process.h"

#include <Windows.h>
#include <TlHelp32.h>

namespace {

void killExe(const wchar_t* name, int& count) {
    HANDLE snap = CreateToolhelp32Snapshot(0x00000002, 0);
    if (snap == INVALID_HANDLE_VALUE) {
        return;
    }

    PROCESSENTRY32W entry = {};
    entry.dwSize = sizeof(entry);

    if (Process32FirstW(snap, &entry)) {
        do {
            if (_wcsicmp(entry.szExeFile, name) != 0) {
                continue;
            }

            HANDLE process = OpenProcess(0x0001, false, entry.th32ProcessID);
            if (!process) {
                continue;
            }

            if (TerminateProcess(process, 0)) {
                count++;
            }

            CloseHandle(process);
        } while (Process32NextW(snap, &entry));
    }

    CloseHandle(snap);
}

}

namespace FNLauncher {

bool closeFortnite(std::wstring& msg) {
    int count = 0;
    killExe(L"FortniteClient-Win64-Shipping.exe", count);

    if (count == 0) {
        msg = L"No Fortnite process was found.";
        return false;
    }

    msg = L"Closed " + std::to_wstring(count) + L" Fortnite process";
    if (count > 1) {
        msg += L"es";
    }
    msg += L".";

    return true;
}

}
