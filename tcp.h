#pragma once

#include "common.h"

namespace tcp
{
	int client(WSADATA& wsaData);
	int server(WSADATA& wsaData);
}
