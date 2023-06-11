#pragma once

#include <cstdint>
#include <string>

namespace Amp {
	class Transpiler
	{
	public:
		static std::string Serialize(const char* src);
	};
}

