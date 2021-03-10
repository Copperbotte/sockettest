// Sockets.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// following this tutorial:
// https://docs.microsoft.com/en-us/windows/win32/winsock/creating-a-basic-winsock-application

#include "tcp.h"

// https://stackoverflow.com/questions/10353017/c-win-async-sockets-is-it-possible-to-interrupt-select

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

    int n = 0;
    printf("type 0 for client, 1 for server:\n");
    cin >> n;

    if(n == 0)
        iResult = tcp::client(wsaData);
    else
        iResult = tcp::server(wsaData);

    WSACleanup();

    return iResult;
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
