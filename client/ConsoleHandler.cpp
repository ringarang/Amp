#include "ConsoleHandler.hpp"

std::string buff;

void ConsoleHandler::Send(const char* fmt, ...)
{
#pragma warning(disable : 4996)
    time_t now = time(0);
    tm* ltm = localtime(&now);

    buff += '[';
    buff += std::to_string(ltm->tm_hour);
    buff += ':';
    buff += std::to_string(ltm->tm_min);
    buff += ':';
    buff += std::to_string(ltm->tm_sec);
    buff += "] ";

    char buffer[10000];
    va_list _ArgList;
    __crt_va_start(_ArgList, fmt);
#pragma warning(push)
#pragma warning(disable: 4996) // Deprecation
    _vsprintf_l(buffer, fmt, NULL, _ArgList);
#pragma warning(pop)
    __crt_va_end(_ArgList);
	buff += buffer;
}

void ConsoleHandler::Write(const char* fmt, ...) {
    char buffer[10000];
    va_list _ArgList;
    __crt_va_start(_ArgList, fmt);
#pragma warning(push)
#pragma warning(disable: 4996) // Deprecation
    _vsprintf_l(buffer, fmt, NULL, _ArgList);
#pragma warning(pop)
    __crt_va_end(_ArgList);
    buff += buffer;
}

const char* ConsoleHandler::GetBuffer()
{
	return buff.c_str();
}

unsigned int ConsoleHandler::GetSize()
{
	return buff.length();
}
