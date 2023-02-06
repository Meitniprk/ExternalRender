//
// Created by ManTkai on 2023/2/6.
//

#include <winsock2.h>
#include <ws2tcpip.h>
#include <cstdlib>
#include <cstdio>

#include "Network.h"
#include "rapidjson/document.h"
#include "Window.h"

namespace Network {
    [[noreturn]] void ConnetServer(std::mutex &documentLock) {
        WSADATA wsaData;
        auto ConnectSocket = INVALID_SOCKET;
        struct addrinfo *result = nullptr, *ptr, hints{};
        const char *sendBuf = "OK";
        int iResult;

        while (true) {
            iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
            if (iResult != 0) {
                printf("WSAStartup failed with error: %d\n", iResult);
                continue;
            }

            ZeroMemory(&hints, sizeof(hints));

            hints.ai_family = AF_UNSPEC;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;

            iResult = getaddrinfo(SERVER_HOST, SERVER_PORT, &hints, &result);
            if (iResult != 0) {
                printf("getAddrInfo failed with error: %d\n", iResult);
                WSACleanup();
                continue;
            }

            for (ptr = result; ptr != nullptr; ptr = ptr->ai_next) {

                ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
                                       ptr->ai_protocol);
                if (ConnectSocket == INVALID_SOCKET) {
                    printf("socket failed with error: %d\n", WSAGetLastError());
                    WSACleanup();
                    break;
                }

                iResult = connect(ConnectSocket, ptr->ai_addr, (int) ptr->ai_addrlen);
                if (iResult == SOCKET_ERROR) {
                    closesocket(ConnectSocket);
                    ConnectSocket = INVALID_SOCKET;
                    continue;
                }
                break;
            }

            freeaddrinfo(result);

            if (ConnectSocket == INVALID_SOCKET) {
                printf("Unable to connect to server!\n");
                WSACleanup();
                continue;
            }

            iResult = send(ConnectSocket, sendBuf, (int) strlen(sendBuf), 0);

            if (iResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ConnectSocket);
                WSACleanup();
                continue;
            }

            iResult = shutdown(ConnectSocket, SD_SEND);
            if (iResult == SOCKET_ERROR) {
                printf("shutdown failed with error: %d\n", WSAGetLastError());
                closesocket(ConnectSocket);
                WSACleanup();
                continue;
            }

            do {
                int len = 0;
                recv(ConnectSocket, (char *) &len, 4, 0);
                len = ntohl(len);
                char *buf = (char *) malloc(len + 1);
                iResult = recv(ConnectSocket, buf, len, 0);
                if (iResult != len) {
                    free(buf);
                    printf("Reconnect\n\r");
                    break;
                }
                buf[len] = '\0';
                rapidjson::Document dom;
                if (!dom.Parse(buf).HasParseError()) {
                    std::lock_guard<std::mutex> documentLock_guard(documentLock);
                    swap(dom, Window::document);
                }
                free(buf);
            } while (true);

            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            WSACleanup();
        }
    }
}