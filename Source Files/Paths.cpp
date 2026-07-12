#include "Paths.h"

#include <Windows.h>
#include <shobjidl.h>

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace FNLauncher {

std::wstring norm(const std::wstring& path) {
    if (path.empty()) {
        return {};
    }

    std::error_code ec;
    return fs::weakly_canonical(fs::path(path), ec).wstring();
}

bool exists(const std::wstring& path) {
    std::error_code ec;
    return fs::exists(path, ec);
}

bool isDir(const std::wstring& path) {
    std::error_code ec;
    return fs::is_directory(path, ec);
}

std::wstring join(const std::wstring& left, const std::wstring& right) {
    return (fs::path(left) / right).wstring();
}

std::wstring baseName(const std::wstring& path) {
    return fs::path(path).filename().wstring();
}

std::wstring trim(const std::wstring& text) {
    size_t start = text.find_first_not_of(L" \t\r\n");
    if (start == std::wstring::npos) {
        return {};
    }

    size_t end = text.find_last_not_of(L" \t\r\n");
    return text.substr(start, end - start + 1);
}

std::wstring configFile() {
    wchar_t appData[MAX_PATH] = {};
    DWORD len = GetEnvironmentVariableW(L"LOCALAPPDATA", appData, MAX_PATH);

    fs::path folder;
    if (len > 0 && len < MAX_PATH) {
        folder = fs::path(appData) / L"FNLauncher";
    } else {
        wchar_t exe[MAX_PATH] = {};
        GetModuleFileNameW(nullptr, exe, MAX_PATH);
        folder = fs::path(exe).parent_path() / L"FNLauncher";
    }

    std::error_code ec;
    fs::create_directories(folder, ec);
    return (folder / L"settings.ini").wstring();
}

std::wstring legacyConfigFile() {
    wchar_t exe[MAX_PATH] = {};
    GetModuleFileNameW(nullptr, exe, MAX_PATH);
    return (fs::path(exe).parent_path() / L"settings.ini").wstring();
}

namespace {

std::wstring browse(bool folder, const std::wstring& title) {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    bool uninit = (hr == S_OK);

    std::wstring picked;
    IFileOpenDialog* picker = nullptr;

    hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&picker));
    if (FAILED(hr)) {
        if (uninit) {
            CoUninitialize();
        }
        return {};
    }

    DWORD opts = FOS_FORCEFILESYSTEM | FOS_NOCHANGEDIR;
    if (folder) {
        opts |= FOS_PICKFOLDERS;
    }
    picker->SetOptions(opts);
    picker->SetTitle(title.c_str());

    if (!folder) {
        COMDLG_FILTERSPEC types[] = {
            {L"DLL files", L"*.dll"},
            {L"All files", L"*.*"},
        };
        picker->SetFileTypes(2, types);
    }

    HWND owner = GetConsoleWindow();
    if (owner) {
        SetForegroundWindow(owner);
    }

    hr = picker->Show(owner);
    if (hr == S_OK) {
        IShellItem* item = nullptr;
        if (SUCCEEDED(picker->GetResult(&item))) {
            PWSTR path = nullptr;
            if (SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path))) {
                picked = path;
                CoTaskMemFree(path);
            }
            item->Release();
        }
    }

    picker->Release();

    if (uninit) {
        CoUninitialize();
    }

    return norm(picked);
}

}

std::wstring pickFolder(const std::wstring& title) {
    return browse(true, title);
}

std::wstring pickDll(const std::wstring& title) {
    return browse(false, title);
}

std::vector<std::wstring> findFile(const std::wstring& root, const std::wstring& name, int depth) {
    std::vector<std::wstring> matches;
    if (!isDir(root) || depth < 0) {
        return matches;
    }

    std::error_code ec;
    for (const auto& entry : fs::recursive_directory_iterator(root, fs::directory_options::skip_permission_denied, ec)) {
        if (ec) {
            break;
        }
        if (!entry.is_regular_file(ec)) {
            continue;
        }
        if (entry.path().filename().wstring() == name) {
            matches.push_back(entry.path().wstring());
        }
    }

    return matches;
}

}
