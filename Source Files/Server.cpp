#include "Server.h"

#include <winsock2.h>
#include <ws2tcpip.h>

#include <cstring>
#include <string>

#pragma comment(lib, "ws2_32.lib")

namespace FNLauncher {

bool pingBackend(const wchar_t* host, int port) {
    WSADATA data;
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
        return false;
    }

    std::wstring portText = std::to_wstring(port);

    addrinfoW hints = {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    addrinfoW* info = nullptr;
    if (GetAddrInfoW(host, portText.c_str(), &hints, &info) != 0) {
        WSACleanup();
        return false;
    }

    SOCKET sock = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
    if (sock == INVALID_SOCKET) {
        FreeAddrInfoW(info);
        WSACleanup();
        return false;
    }

    DWORD timeout = 2000;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));

    bool connected = connect(sock, info->ai_addr, (int)info->ai_addrlen) != SOCKET_ERROR;
    if (!connected) {
        closesocket(sock);
        FreeAddrInfoW(info);
        WSACleanup();
        return false;
    }

    const char request[] =
        "GET /unknown HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Connection: close\r\n"
        "\r\n";

    send(sock, request, (int)strlen(request), 0);

    char buffer[128] = {};
    int got = recv(sock, buffer, sizeof(buffer) - 1, 0);

    closesocket(sock);
    FreeAddrInfoW(info);
    WSACleanup();

    return got > 0;
}

}
