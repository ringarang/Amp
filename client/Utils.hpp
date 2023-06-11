#pragma once

#include <string>

namespace Amp {
	class Utils
	{
	public:
		static unsigned int Rebase(unsigned int addr = 0x0, int nbase = 0x400000);

		static void SetByte(unsigned int addr, std::uint8_t v);
		static std::uint8_t GetByte(unsigned int addr);
		static std::string ResolveHex(std::string hex);
		static std::string GetCurrentDirectoryPath();
		static void alua_getservice(unsigned long ls, const char* service);
		static bool alua_isaservice(const char* service);
	};
}

