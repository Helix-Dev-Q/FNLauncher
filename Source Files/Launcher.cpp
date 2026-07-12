#include "Launcher.h"
#include "Paths.h"

#include <Windows.h>

#include <filesystem>
#include <sstream>
#include <vector>

namespace fs = std::filesystem;

namespace {

std::wstring quote(const std::wstring& text) {
    if (text.find(L' ') == std::wstring::npos && text.find(L'"') == std::wstring::npos) {
        return text;
    }

    std::wstring out = L"\"";
    for (wchar_t ch : text) {
        if (ch == L'"') {
            out += L"\\\"";
        } else {
            out += ch;
        }
    }
    out += L'"';
    return out;
}

std::wstring makeArgs(const std::wstring& exe, const std::wstring& email, const std::wstring& password) {
    std::wostringstream out;
    out << quote(exe) << L" ";
    out << L"-epicapp=Fortnite ";
    out << L"-epicenv=Prod ";
    out << L"-epiclocale=en-us ";
    out << L"-epicportal ";
    out << L"-skippatchcheck ";
    out << L"-nobe ";
    out << L"-fromfl=eac ";
    out << L"-fltoken=3db3ba5dcbd2e16703f3978d ";
    out << L"-caldera=eyJhbGciOiJFUzI1NiIsInR5cCI6IkpXVCJ9.eyJhY2NvdW50X2lkIjoiYmU5ZGE1YzJmYmVhNDQwN2IyZjQwZWJhYWQ4NTlhZDQiLCJnZW5lcmF0ZWQiOjE2Mzg3MTcyNzgsImNhbGRlcmFHdWlkIjoiMzgxMGI4NjMtMmE2NS00NDU3LTliNTgtNGRhYjNiNDgyYTg2IiwiYWNQcm92aWRlciI6IkVhc3lBbnRpQ2hlYXQiLCJub3RlcyI6IiIsImZhbGxiYWNrIjpmYWxzZX0.VAWQB67RTxhiWOxx7DBjnzDnXyyEnX7OljJm-j2d88G_WgwQ9wrE6lwMEHZHjBd1ISJdUO1UVUqkfLdU5nofBQ ";
    out << L"-AUTH_LOGIN=" << quote(email) << L" ";
    out << L"-AUTH_PASSWORD=" << quote(password) << L" ";
    out << L"-AUTH_TYPE=epic";
    return out.str();
}

}

namespace FNLauncher {

bool startFortnite(
    const Build& build,
    const std::wstring& email,
    const std::wstring& password,
    unsigned long& pid,
    std::wstring& err) {
    if (build.exe.empty() || !exists(build.exe)) {
        err = L"Executable missing.";
        return false;
    }

    std::wstring exe = norm(build.exe);
    std::wstring folder = fs::path(exe).parent_path().wstring();
    std::wstring login = email;
    std::wstring pass = password.empty() ? L"Rebooted" : password;
    std::wstring args = makeArgs(exe, login, pass);

    STARTUPINFOW start = {};
    start.cb = sizeof(start);

    PROCESS_INFORMATION proc = {};

    _wputenv_s(L"OPENSSL_ia32cap", L"~0x20000000");

    std::vector<wchar_t> line(args.begin(), args.end());
    line.push_back(L'\0');

    if (!CreateProcessW(exe.c_str(), line.data(), nullptr, nullptr, false, 0, nullptr, folder.c_str(), &start, &proc)) {
        err = L"Could not start Fortnite.";
        return false;
    }

    pid = proc.dwProcessId;
    CloseHandle(proc.hThread);
    CloseHandle(proc.hProcess);
    return true;
}

}
