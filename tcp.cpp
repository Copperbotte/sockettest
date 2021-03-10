
#include "tcp.h"

int tcp::server(WSADATA& wsaData)
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
    if (iResult == SOCKET_ERROR)
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

    // While a user hasn't sent the /shutdown command:
    // check for new connections and messages
    // on new connection:
    //    create the new connection, add the socket to the socket array
    // on new message
    //    echo message to all connected clients
    // on socket connection terminated
    //    remove from socket array, push all other sockets back one?
    //        maybe use a linked list instead

    printf("Waiting for a new connection\n");

    int connections = 0;

    fd_set ls;
    ls.fd_count = 1;
    ls.fd_array[0] = ListenSocket;

    fd_set cs;
    cs.fd_count = 0;

    fd_set ms;
    cs.fd_count = 0;

    timeval time;
    time.tv_sec = 1;
    time.tv_usec = 0;

    while (true)
    {
        //check for new connections
        ls.fd_count = 1; // always update the listen socket
        iResult = select(ls.fd_count, &ls, 0, 0, &time); // final null is blocking
        //printf("%d\n", iResult);
        if (iResult == SOCKET_ERROR)
        {

            printf("select failed with error: %1d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }

        //establish connections
        if (0 < iResult)
        {
            for (int i = 0; i < iResult; ++i)
                cs.fd_array[connections + i] = accept(ListenSocket, NULL, NULL);
            memcpy(ms.fd_array, cs.fd_array, sizeof(SOCKET*) * 64);
            connections += iResult;
            printf("Found %d new connections.", iResult);
        }

        if (connections == 0)
            continue;

        //check for new messages
        ms.fd_count = connections; // always update the listen socket
        iResult = select(ms.fd_count, &ms, 0, 0, &time); // final null is blocking
        if (iResult == SOCKET_ERROR)
        {
            printf("select failed with error: %1d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }

        if (iResult == 0)
            continue;
        //printf("%d connections ", connections);
        //printf("%d new messages\n", iResult);

        //parse messages and echo them out to all other connected clients
        const int recvBufferLen = 1024;
        char recvBuffer[recvBufferLen];

        for (int i = 0; i < iResult; ++i)
        {
            //recieve message
            recv(ms.fd_array[i], recvBuffer, recvBufferLen, 0);
            printf("%s\n", recvBuffer);

            //printf("recieved from connection %d\n", i);

            //echo for everyone else
            for (int j = 0; j < connections; ++j)
            {
                if (ms.fd_array[i] == cs.fd_array[j])
                    continue; // dont echo the message
                send(cs.fd_array[j], recvBuffer, recvBufferLen, 0);
            }
        }
    }

    //ClientSocket = accept(ListenSocket, NULL, NULL);
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

    //socketChat(ClientSocket);

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

int tcp::client(WSADATA& wsaData)
{
    // https://docs.microsoft.com/en-us/windows/win32/winsock/creating-a-socket-for-the-client
    addrinfo* result = nullptr;
    addrinfo* ptr = nullptr;
    addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    printf("Enter an address:\n");
    string s = "127.0.0.1";
    cin >> s;

    int iResult = getaddrinfo(s.c_str(), "27015", &hints, &result);
    if (iResult != 0)
    {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // setup tcp listen socket
    SOCKET ConnectSocket = INVALID_SOCKET;
    ConnectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET)
    {
        printf("Error at socket(): %1d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // https://docs.microsoft.com/en-us/windows/win32/winsock/connecting-to-a-socket
    // connect to server
    iResult = connect(ConnectSocket, result->ai_addr, result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }

    //addrinfo is no longer needed
    freeaddrinfo(result);
    if (ConnectSocket == INVALID_SOCKET)
    {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    // https://docs.microsoft.com/en-us/windows/win32/winsock/sending-and-receiving-data-on-the-client

    //socketChat(ConnectSocket);

    const int recvBufferLen = 1024;
    char recvBuffer[recvBufferLen];

    while (true)
    {
        //check if we recieved any messages
        fd_set ls;
        ls.fd_count = 1;
        ls.fd_array[0] = ConnectSocket;

        timeval time;
        time.tv_sec = 1;
        time.tv_usec = 0;

        iResult = select(ls.fd_count, &ls, 0, 0, &time); // final null is blocking
        if (iResult == SOCKET_ERROR)
        {
            printf("select failed with error: %1d\n", WSAGetLastError());
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }

        //new messages!!!!
        if (0 < iResult)
        {
            for (int i = 0; i < ls.fd_count; ++i)
            {
                recv(ls.fd_array[i], recvBuffer, recvBufferLen, 0);
                printf("%s\n", recvBuffer);
            }
        }

        if (GetKeyState(VK_SPACE) & 0x8000)
        {
            printf("Send somethin!\n>>> ");
            s = "";
            cin >> s;
            s += '\0';
            send(ConnectSocket, s.c_str(), s.length(), 0);
        }

    }

    //recieve message
    recv(ConnectSocket, recvBuffer, recvBufferLen, 0);
    printf(recvBuffer);


    // https://docs.microsoft.com/en-us/windows/win32/winsock/disconnecting-the-client
    // send shutdown message
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}