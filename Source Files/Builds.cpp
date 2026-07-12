#include "Builds.h"
#include "Paths.h"

namespace FNLauncher {

bool validRoot(const std::wstring& root) {
    std::wstring path = norm(root);
    return isDir(join(path, L"FortniteGame")) && isDir(join(path, L"Engine"));
}

std::optional<std::wstring> findExe(const std::wstring& root, std::wstring& err) {
    if (!validRoot(root)) {
        err = L"This folder does not contain FortniteGame and Engine.";
        return std::nullopt;
    }

    auto matches = findFile(root, L"FortniteClient-Win64-Shipping.exe", 8);
    if (matches.empty()) {
        err = L"FortniteClient-Win64-Shipping.exe not found.";
        return std::nullopt;
    }
    if (matches.size() > 1) {
        err = L"Multiple shipping executables found. Pick a cleaner build folder.";
        return std::nullopt;
    }

    return matches.front();
}

std::optional<Build> addBuild(const std::wstring& root, std::wstring& err) {
    std::wstring path = norm(root);
    if (!validRoot(path)) {
        err = L"This folder does not contain FortniteGame and Engine.";
        return std::nullopt;
    }

    auto exe = findExe(path, err);
    if (!exe) {
        return std::nullopt;
    }

    Build build;
    build.name = baseName(path);
    build.root = path;
    build.exe = *exe;
    return build;
}

}
