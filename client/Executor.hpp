#pragma once

#include <string>
#include "Roblox.hpp"

namespace Amp {
	namespace Executor
	{
		const char* DoString(Roblox::lua_State* ls, std::string src);
	};
}
