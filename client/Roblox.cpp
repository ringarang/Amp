#include "Roblox.hpp"

#include <exception>

#include "Amp.hpp"
#include "ScriptScheduler.hpp"

Roblox::lua_State* scriptContextLuaState;
uintptr_t datamodel, scriptContext;

const char* Roblox::Scan()
{
	try {
		/* Get Datamodel */
		int pad0[2];
		void* dataModelThis = GetDataModelThis();
		GetDataModel(dataModelThis, pad0);
		datamodel = pad0[0] + getDatamodelOffset;

		/* Get ScriptContext */
		scriptContext = Instance(datamodel).FindFirstChildOfClass("ScriptContext").GetRawInstance();

		/* Get LuaState */
		uintptr_t luaState = GET_LS(scriptContext, 0);
		/* Change identity to 7 */
		SetContext(reinterpret_cast<Roblox::lua_State*>(luaState), 7);
		/* New thread */
		scriptContextLuaState = lua_newthread(reinterpret_cast<Roblox::lua_State*>(luaState));	
		/* Mark this thread */
		MarkState(scriptContextLuaState);

		//Amp::ScriptScheduler::Connect(scriptContextLuaState);
		
		return nullptr;
	}
	catch (std::exception e) {
		return e.what();
	}
	catch (...) {
		return "Unknown error";
	}
}

Roblox::lua_State* Roblox::GetLuaState()
{
	return scriptContextLuaState;
}

bool Roblox::IsStateMarked(Roblox::lua_State* ls)
{
	uintptr_t lSI1 = *reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ls) + identity1);
	uintptr_t* pLSI2 = reinterpret_cast<uintptr_t*>(lSI1 + identity2);
	return *pLSI2 == Amp::seed;
}

void Roblox::MarkState(Roblox::lua_State* ls)
{
	uintptr_t lSI1 = *reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ls) + identity1);
	uintptr_t* pLSI2 = reinterpret_cast<uintptr_t*>(lSI1 + identity2);
	*pLSI2 = Amp::seed;
}

void Roblox::UnmarkState(Roblox::lua_State* ls)
{
	uintptr_t lSI1 = *reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ls) + identity1);
	uintptr_t* pLSI2 = reinterpret_cast<uintptr_t*>(lSI1 + identity2);
	*pLSI2 = 0;
}

std::uint32_t Roblox::GetContext(Roblox::lua_State* ls)
{
	uintptr_t lSI1 = *reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ls) + identity1);
	uintptr_t* pLSI2 = reinterpret_cast<uintptr_t*>(lSI1 + identity2);
	return *pLSI2;
}

void Roblox::SetContext(Roblox::lua_State* ls, std::uintptr_t v)
{
	uintptr_t lSI1 = *reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ls) + identity1);
	uintptr_t* pLSI2 = reinterpret_cast<uintptr_t*>(lSI1 + identity2);
	*pLSI2 = v;
}

Roblox::Instance::Instance(int a)
{
	instance = a;
}

const char* Roblox::Instance::GetClass()
{
	return (const char*)(*(uintptr_t(**)(void))(*(uintptr_t*)instance + 16))();
}

Roblox::Instance Roblox::Instance::FindFirstChildOfClass(const char* name)
{
	uintptr_t StartOfChildren = *(uintptr_t*)(instance + 0x2C);
	uintptr_t EndOfChildren = *(uintptr_t*)(StartOfChildren + 4);

	for (uintptr_t i = *(uintptr_t*)StartOfChildren; i != EndOfChildren; i += 8)
	{
		Instance inst(*(uintptr_t*)i);
		if (memcmp(inst.GetClass(), name, strlen(name)) == 0)
		{
			return inst;
		}
	}

	return 0;
}

int Roblox::Instance::GetRawInstance()
{
	return instance;
}
