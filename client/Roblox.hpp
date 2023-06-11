#pragma once

#include "Addresses.hpp"

#include <cstdint>

namespace Roblox
{
	const char* Scan();

	Roblox::lua_State* GetLuaState();

	bool IsStateMarked(Roblox::lua_State* ls);
	void MarkState(Roblox::lua_State* ls);
	void UnmarkState(Roblox::lua_State* ls);

	std::uint32_t GetContext(Roblox::lua_State* ls);
	void SetContext(Roblox::lua_State* ls, std::uint32_t v);

	class Instance {
	private:
		int instance;
	public:
		Instance(int a);

		const char* GetClass();
		Instance FindFirstChildOfClass(const char* name);

		int GetRawInstance();
	};
};

