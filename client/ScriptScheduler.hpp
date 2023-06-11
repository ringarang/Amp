#pragma once

#include "Roblox.hpp"

namespace Amp {
	class ScriptScheduler
	{
	public:
		static void Connect(Roblox::lua_State* ls);

		static void PushScript(const char* src);
	};
}

