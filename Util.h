#pragma once
#define WIN32_LEAN_AND_MEAN


#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <exception>
#include <iostream>
#include <string>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
// "27015"
#define DEFAULT_PORT "55054" 


using namespace std;

class Util
{
public:
    // #include "web_client_2.cpp"
    static bool char_to_wchar(const char* c, wchar_t* buf, size_t max_length = 255)
    {
        const size_t cSize = strlen(c) + 1;
        if (cSize >= max_length) return false;
        return mbstowcs_s(&max_length, buf, cSize, c, strlen(c));
    }

    static bool wchar_to_char(const wchar_t* wc, char* buf, int max_length = 255)
    {
        size_t out_size = max_length;
        size_t in_size = max_length;
        return  wcstombs_s(&out_size, buf, max_length, wc, out_size);
    }


    static string get_err_msg(int err)
    {
        char* msgbuf = nullptr;
        try {
            char* msgbuf = new char[512];
            msgbuf[0] = '\0';    // Microsoft doesn't guarantee this on man page.

            wchar_t wmsgbuf[512];

            Util::char_to_wchar(msgbuf, wmsgbuf, 512);

            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,   // flags
                NULL,                                           // lpsource
                err,                                            // message id
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),      // languageid
                wmsgbuf,                                        // output buffer
                sizeof(wmsgbuf),                                // size of msgbuf, bytes
                NULL                                            // va_list of arguments
            );

            Util::wchar_to_char(wmsgbuf, msgbuf);
            string return_string(msgbuf);
            if (msgbuf != nullptr) delete[] msgbuf;
            return return_string;
        }
        catch (exception& e) {
            cout << endl << "Fehler in get_err_msg()! e.what(): " << e.what();
        }
        catch (...) {
            cout << endl << "Unbekannter Fehler in get_err_msg()!";
        }
        if (msgbuf != nullptr) delete[] msgbuf;
        return string("");
    }

};


/*
int client_1(int argc, char** argv)
{
    try {
        // const char* server_name = "94.126.18.110";
        const char* server_name = "localhost";

        WSADATA wsaData;
        SOCKET ConnectSocket = INVALID_SOCKET;
        struct addrinfo* result = NULL, * ptr = NULL, hints;
        const char* sendbuf = "GET / index.php HTTP / 1.1\r\n""Host: "".ch\r\n""\r\n";

        char recvbuf[DEFAULT_BUFLEN];
        int iResult;
        int recvbuflen = DEFAULT_BUFLEN;



        // Initialize Winsock
        iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            printf("WSAStartup failed with error: %d\n", iResult);
            return 1;
        }

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        // Resolve the server address and port
        iResult = getaddrinfo(server_name, DEFAULT_PORT, &hints, &result);
        if (iResult != 0) {
            printf("getaddrinfo failed with error: %d\n", iResult);

            WSACleanup();
            return 1;
        }

        // Attempt to connect to an address until one succeeds
        for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

            // Create a SOCKET for connecting to server
            ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
                ptr->ai_protocol);
            if (ConnectSocket == INVALID_SOCKET) {
                printf("socket failed with error: %ld\n", WSAGetLastError());
                WSACleanup();
                return 1;
            }

            // Connect to server.
            iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
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
            int err = WSAGetLastError();
            printf("Fehler Code: %d", err);
            char buf[255];
            Util::get_err_msg(err, buf);
            printf("Fehler: %s", buf);
            return 1;
        }

        // Send an initial buffer
        iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
        if (iResult == SOCKET_ERROR) {
            printf("send failed with error: %d\n", WSAGetLastError());
            char buf[255];
            Util::get_err_msg(WSAGetLastError(), buf);
            printf("Fehler: %s", buf);
            closesocket(ConnectSocket);
            return 1;
        }

        printf("Bytes Sent: %ld\n", iResult);

        // shutdown the connection since no more data will be sent
        iResult = shutdown(ConnectSocket, SD_SEND);
        if (iResult == SOCKET_ERROR) {
            int err = WSAGetLastError();
            char buf[255];
            Util::get_err_msg(WSAGetLastError(), buf);
            printf("Fehler: %s", buf);
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }

        // Receive until the peer closes the connection
        do {

            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
            if (iResult > 0)
                printf("Bytes received: %d\n", iResult);
            else if (iResult == 0)
                printf("Connection closed\n");
            else
                printf("recv failed with error: %d\n", WSAGetLastError());

        } while (iResult > 0);

        // cleanup
        closesocket(ConnectSocket);
        WSACleanup();
    }
    catch (std::exception& e) {
        std::cout << std::endl << e.what();
    }

    return 0;
}

int client_2(int argc, char** argv)
{
    int sock = 0, valread, client_fd;
    struct sockaddr_in serv_addr;
    string msg_to_server = "GET / index.php HTTP / 1.1\r\n""Host: " + dest_server + "\r\n""\r\n";
    const char* sendbuf = msg_to_server.c_str();

    char buffer[1024] = { 0 };
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary
    // form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)
        <= 0) {
        printf(
            "\nInvalid address/ Address not supported \n");
        return -1;
    }

    if ((client_fd
        = connect(sock, (struct sockaddr*)&serv_addr,
            sizeof(serv_addr)))
        < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }
    send(sock, sendbuf, strlen(sendbuf), 0);
    printf("Hello message sent\n");
    valread = recv(sock, buffer, 1024, 0);
    printf("%s\n", buffer);

    // closing the connected socket
    closesocket(client_fd);
    return 0;
}

*/