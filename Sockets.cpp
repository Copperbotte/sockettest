// Sockets.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// following this tutorial:
// https://docs.microsoft.com/en-us/windows/win32/winsock/creating-a-basic-winsock-application

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>

#include <winsock2.h> // sockets
#include <WS2tcpip.h> // winsock 2 tcpip?
#include <iphlpapi.h> // ip help api? // lean and mean

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int server(WSADATA& wsaData)
{
    // https://docs.microsoft.com/en-us/windows/win32/winsock/creating-a-socket-for-the-server
    addrinfo* result = nullptr;
    addrinfo* ptr = nullptr;
    addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    int iResult = getaddrinfo(NULL, "27015", &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // setup tcp listen socket
    SOCKET ListenSocket = INVALID_SOCKET;
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
    {
        printf("Error at socket(): %1d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // https://docs.microsoft.com/en-us/windows/win32/winsock/binding-a-socket
    // bind socket to network address
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if(iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %1d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // addrinfo is only needed for bind
    freeaddrinfo(result);

    // https://docs.microsoft.com/en-us/windows/win32/winsock/listening-on-a-socket

    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        printf("Listen failed with error: %1d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // https://docs.microsoft.com/en-us/windows/win32/winsock/accepting-a-connection
    SOCKET ClientSocket = INVALID_SOCKET;

    // accept client socket
    // program will wait here until a valid connection appears
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET)
    {
        printf("accept failed: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // No longer need to listen for more connections
    closesocket(ListenSocket); 

    printf("connection found\n");

    // https://docs.microsoft.com/en-us/windows/win32/winsock/receiving-and-sending-data-on-the-server

    // https://docs.microsoft.com/en-us/windows/win32/winsock/disconnecting-the-server

    iResult = shutdown(ClientSocket, SD_SEND); // sends a shutdown command to the client
    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }


    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}

int main()
{
    int iResult = 0;

    //initialize winsock
    WSADATA wsaData;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        printf("WSAStartup Failed: %d\n", iResult);
        return 1;
    }

    return server(wsaData);

    WSACleanup();

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
