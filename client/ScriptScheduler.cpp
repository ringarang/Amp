/* note:
* So, what is a ScriptScheduler you may ask?
* 
* That's what people may call a "Task Scheduler", or a "Thread Scheduler", but I prefer calling it a Script Scheduler (fits way more to what it really does)
*/

#include "ScriptScheduler.hpp"

#include "Addresses.hpp"
#include "Roblox.hpp"
#include "Executor.hpp"
#include "Utils.hpp"
#include "ConsoleHandler.hpp"

#include <vector>
#include <queue>

std::queue<const char*> srcs = {};

int f_rndrstppd(Roblox::lua_State* ls) {
    if (!srcs.empty()) {
        const char* src = srcs.front(); // Get the source code at the bottom of the vector

        Roblox::lua_State* nt0 = Roblox::lua_newthread(Roblox::GetLuaState());
        Roblox::SetContext(nt0, 7);
        Roblox::MarkState(nt0);
        Roblox::lua_remove(Roblox::GetLuaState(), -1); // NOTE: When creating a new thread, it gets pushed as a Value on the original stack; remove it
        const char* ds0 = Amp::Executor::DoString(nt0, src);
        if (ds0 != nullptr) Roblox::singletonPrint(3, ds0);

        srcs.pop();
    }

    return 0;
}

void Amp::ScriptScheduler::Connect(Roblox::lua_State* ls)
{
    /* Get RunService */    
    rlua_getglobal(ls, "game");
    Roblox::lua_getfield(ls, -1, "GetService"); 
    Roblox::lua_pushvalue(ls, -2);
    rlua_pushstring(ls, "RunService");
    int res = Roblox::lua_pcall(ls, 2, 1, 0);
    if (res) {
        ConsoleHandler::Send("%s\n", Roblox::lua_tolstring(ls, -1, 0));
        throw std::exception("ScriptScheduler failed to get RunService service");
    }

    /* Connect RenderStepped */
    Roblox::lua_getfield(ls, -1, "RenderStepped");
    Roblox::lua_getfield(ls, -1, "Connect");
    Roblox::lua_pushvalue(ls, -2);
    rlua_pushcfunction(ls, f_rndrstppd);
    res = Roblox::lua_pcall(ls, 2, 1, 0);
    if (res) {
        throw std::exception("ScriptScheduler failed to connect to RenderStepped.Connect event");
    }

    /* Clear the stack */
    rlua_clearstack(ls); 
}

void Amp::ScriptScheduler::PushScript(const char* src)
{
	srcs.push(src);
}
