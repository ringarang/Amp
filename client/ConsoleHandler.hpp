#pragma once

#include <string>
#include <ctime>

class ConsoleHandler
{
public:
	static void Send(const char* fmt, ...);
	static void Write(const char* fmt, ...);

	static const char* GetBuffer();
	static unsigned int GetSize();
};

