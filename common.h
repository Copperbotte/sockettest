#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include <string>

#include <winsock2.h> // sockets
#include <WS2tcpip.h> // winsock 2 tcpip?
#include <iphlpapi.h> // ip help api? // lean and mean

#pragma comment(lib, "Ws2_32.lib")

using namespace std;
