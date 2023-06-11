#include "Executor.hpp"

#include "Transpiler.hpp"
#include "Roblox.hpp"
#include "ConsoleHandler.hpp"

const char* Amp::Executor::DoString(Roblox::lua_State* ls, std::string src)
{
	try {
		const char* ret = nullptr;

		const std::string b = Amp::Transpiler::Serialize(src.c_str());

		int res = Roblox::luau_load(ls, "amp", b.c_str(), b.length());
		if (res == 0) Roblox::lua_spawn(ls);
		else {			
			const char* errmsg = Roblox::lua_tolstring(ls, -1, NULL);			
			ConsoleHandler::Send("Couldn't execute :(.\n\t%s\n", errmsg);
			ret = errmsg;
		}
		Roblox::lua_settop(ls, 0);
		return ret;
	}
	catch (std::exception e) {
		return e.what();
	}
	catch (...) {
		return "Unknown error - really descriptive";
	}
}
