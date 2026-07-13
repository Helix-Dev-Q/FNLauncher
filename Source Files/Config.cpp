#include "Config.h"
#include "Paths.h"

#include <algorithm>
#include <fstream>

namespace {

using namespace FNLauncher;

std::wstring escape(const std::wstring& text) {
    std::wstring out;
    for (wchar_t ch : text) {
        if (ch == L'\\') {
            out += L"\\\\";
        } else if (ch == L'\n') {
            out += L"\\n";
        } else {
            out += ch;
        }
    }
    return out;
}

std::wstring unescape(const std::wstring& text) {
    std::wstring out;
    for (size_t i = 0; i < text.size(); i++) {
        if (text[i] == L'\\' && i + 1 < text.size()) {
            if (text[i + 1] == L'n') {
                out += L'\n';
                i++;
            } else {
                out += text[i + 1];
                i++;
            }
        } else {
            out += text[i];
        }
    }
    return out;
}

std::wstring valueOf(const std::wstring& line) {
    size_t split = line.find(L'=');
    if (split == std::wstring::npos) {
        return {};
    }
    return unescape(trim(line.substr(split + 1)));
}

}

namespace FNLauncher {

std::wstring effectiveEmail(const std::wstring& email) {
    return email.empty() ? defaultEmail() : email;
}

std::wstring effectivePassword(const std::wstring& password) {
    return password.empty() ? defaultPassword() : password;
}

std::wstring effectiveClientEmail(const std::wstring& email) {
    return email.empty() ? defaultClientEmail() : email;
}

std::wstring effectiveClientPassword(const std::wstring& password) {
    return password.empty() ? defaultClientPassword() : password;
}

bool usingDefaultEmail(const std::wstring& email) {
    return email.empty();
}

bool usingDefaultPassword(const std::wstring& password) {
    return password.empty();
}

bool usingDefaultClientEmail(const std::wstring& email) {
    return email.empty();
}

bool usingDefaultClientPassword(const std::wstring& password) {
    return password.empty();
}

bool loadConfig(Config& cfg, std::wstring&) {
    std::wstring file = configFile();
    std::wstring from = file;

    if (!exists(file) && exists(legacyConfigFile())) {
        from = legacyConfigFile();
    }

    std::wifstream in(from);
    if (!in) {
        return true;
    }

    cfg = Config{};
    std::wstring line;

    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty() || line[0] == L'#') {
            continue;
        }

        if (line.rfind(L"clientemail=", 0) == 0) {
            cfg.clientEmail = valueOf(line);
        } else if (line.rfind(L"clientpassword=", 0) == 0) {
            cfg.clientPassword = valueOf(line);
        } else if (line.rfind(L"email=", 0) == 0) {
            cfg.email = valueOf(line);
        } else if (line.rfind(L"password=", 0) == 0) {
            cfg.password = valueOf(line);
        } else if (line.rfind(L"console=", 0) == 0) {
            cfg.consoleDll = valueOf(line);
        } else if (line.rfind(L"gameserverdll=", 0) == 0) {
            cfg.gameServerDll = valueOf(line);
        } else if (line.rfind(L"port=", 0) == 0) {
            cfg.port = std::stoi(valueOf(line));
        } else if (line.rfind(L"active=", 0) == 0) {
            cfg.active = std::stoi(valueOf(line));
        } else if (line.rfind(L"activedll=", 0) == 0) {
            cfg.activeDll = std::stoi(valueOf(line));
        } else if (line.rfind(L"dll.", 0) == 0) {
            size_t split = line.find(L'=');
            if (split == std::wstring::npos) {
                continue;
            }
            int index = std::stoi(line.substr(4, split - 4));
            if (index >= (int)cfg.dlls.size()) {
                cfg.dlls.resize(index + 1);
            }
            cfg.dlls[index] = valueOf(line);
        } else if (line.rfind(L"dll=", 0) == 0) {
            std::wstring path = valueOf(line);
            if (!path.empty()) {
                cfg.dlls.push_back(path);
                if (cfg.activeDll < 0) {
                    cfg.activeDll = 0;
                }
            }
        } else if (line.rfind(L"build.", 0) == 0) {
            size_t dot = line.find(L'.', 6);
            size_t split = line.find(L'=', dot);
            if (dot == std::wstring::npos || split == std::wstring::npos) {
                continue;
            }

            int index = std::stoi(line.substr(6, dot - 6));
            std::wstring key = line.substr(dot + 1, split - dot - 1);
            std::wstring val = valueOf(line);

            if (index >= (int)cfg.builds.size()) {
                cfg.builds.resize(index + 1);
            }

            if (key == L"name") {
                cfg.builds[index].name = val;
            } else if (key == L"root") {
                cfg.builds[index].root = val;
            } else if (key == L"exe") {
                cfg.builds[index].exe = val;
            }
        }
    }

    cfg.dlls.erase(std::remove(cfg.dlls.begin(), cfg.dlls.end(), std::wstring{}), cfg.dlls.end());
    cfg.builds.erase(
        std::remove_if(cfg.builds.begin(), cfg.builds.end(), [](const Build& b) { return b.root.empty(); }),
        cfg.builds.end());

    if (cfg.active >= (int)cfg.builds.size()) {
        cfg.active = cfg.builds.empty() ? -1 : 0;
    }
    if (cfg.activeDll >= (int)cfg.dlls.size()) {
        cfg.activeDll = cfg.dlls.empty() ? -1 : 0;
    }

    if (from != file) {
        std::wstring ignored;
        saveConfig(cfg, ignored);
    }

    return true;
}

bool saveConfig(const Config& cfg, std::wstring& err) {
    std::wofstream out(configFile(), std::ios::trunc);
    if (!out) {
        err = L"Could not write settings.";
        return false;
    }

    out << L"email=" << escape(cfg.email) << L"\n";
    out << L"password=" << escape(cfg.password) << L"\n";
    out << L"clientemail=" << escape(cfg.clientEmail) << L"\n";
    out << L"clientpassword=" << escape(cfg.clientPassword) << L"\n";
    out << L"console=" << escape(cfg.consoleDll) << L"\n";
    out << L"gameserverdll=" << escape(cfg.gameServerDll) << L"\n";
    out << L"port=" << cfg.port << L"\n";
    out << L"active=" << cfg.active << L"\n";
    out << L"activedll=" << cfg.activeDll << L"\n";

    for (size_t i = 0; i < cfg.dlls.size(); i++) {
        out << L"dll." << i << L"=" << escape(cfg.dlls[i]) << L"\n";
    }

    for (size_t i = 0; i < cfg.builds.size(); i++) {
        out << L"build." << i << L".name=" << escape(cfg.builds[i].name) << L"\n";
        out << L"build." << i << L".root=" << escape(cfg.builds[i].root) << L"\n";
        out << L"build." << i << L".exe=" << escape(cfg.builds[i].exe) << L"\n";
    }

    return true;
}

}
