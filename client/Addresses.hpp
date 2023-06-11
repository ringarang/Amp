#pragma once

#include "Addressing.hpp"

#include "Utils.hpp"
#include "Callcheck.hpp"

/* : If you don't understand:

	NEW_ADDRESS(ADDRESS_NAME, ADDRESS_LOCATION) -- Address location will be automatically rebased
	NEW_OFFSET(OFFSET_NAME, OFFSET_VALUE) -- Simply creates a new unsigned int variable named OFFSET_NAME with OFFSET_VALUE as value.
	NEW_CONST_OFFSET(OFFSET_NAME, OFFSET_VALUE) -- Does the same as NEW_OFFSET, but the variable is constant.
	NEW_SUBROUTINE(SUB_NAME, SUB_LOCATION, SUB_CALL_CONV, SUB_CALL_RETURN_TYPE, VARIADIC: ALL ARGUMENTS TYPE) -- Handles retcheck, rebasing and call conventions automatically. you can just do SUB_NAME(ARGS); now!

*/

/*-- EVERYTHING TO UPDATE -- START --*/
namespace Roblox {
	/*
	Generated using amp: structgen.
	*/

	struct lua_State {
		std::uint8_t pad0[12]; // covers 0 -> 12
		int Top; // *reinterpret_cast<int*>(a1 + 12);
		int Base; // *reinterpret_cast<int*>(a1 + 16);
	};

	using lua_CFunction = int (*) (Roblox::lua_State* ls);

#define GET_LS(sc, ms) (sc + 0xB4) ^ *(uintptr_t*)(sc + 0xB4);
	NEW_OFFSET(identity1, 104);
	NEW_OFFSET(identity2, 24);

	NEW_OFFSET(getDatamodelOffset, 0xC);
	NEW_SUBROUTINE(GetDataModel, 0xF24A80, __thiscall, int, void*, int*);
	NEW_SUBROUTINE(GetDataModelThis, 0xF24930, __cdecl, void*, void);
	NEW_SUBROUTINE(singletonPrint, 0x65D8E0, __cdecl, int, int, const char*);
	NEW_SUBROUTINE(luau_load, 0x143EDD0, __cdecl, int, Roblox::lua_State*, const char*, const char*, unsigned int);
	NEW_SUBROUTINE(lua_spawn, 0x79C4F0, __cdecl, void, Roblox::lua_State*);
	NEW_SUBROUTINE(lua_newthread, 0x1433ED0, __cdecl, Roblox::lua_State*, Roblox::lua_State*);
	NEW_SUBROUTINE(lua_gettop, 0x1433BA0, __cdecl, int, Roblox::lua_State*);
	NEW_SUBROUTINE(lua_getfield, 0x14339D0, __fastcall, void, Roblox::lua_State*, int, const char*);
	NEW_SUBROUTINE(lua_setfield, 0x1434E30, __stdcall, void, Roblox::lua_State*, int, const char*);
	NEW_SUBROUTINE(lua_pushlstring, 0x1449180, __cdecl, void, Roblox::lua_State*, const char*, unsigned int);
	NEW_SUBROUTINE(lua_pushnumber, 0x1434520, __cdecl, void, Roblox::lua_State*, double);
	NEW_SUBROUTINE(lua_pushboolean, 0x1434200, __cdecl, void, Roblox::lua_State*, int);
	NEW_SUBROUTINE(lua_pushnil, 0x14344D0, __cdecl, void, Roblox::lua_State*);
	NEW_SUBROUTINE(lua_pushvalue, 0x1434730, __cdecl, void, Roblox::lua_State*, int);
	NEW_SUBROUTINE(o_lua_pushcclosure, 0x1434250, __cdecl, void, Roblox::lua_State*, lua_CFunction, int, int, int);
	NEW_SUBROUTINE(lua_pcall, 0x1434140, __cdecl, int, Roblox::lua_State*, int, int, int);
	NEW_SUBROUTINE(lua_call, 0x1433500, __cdecl, int, Roblox::lua_State*, int, int);
	NEW_SUBROUTINE(lua_remove, 0x1434BD0, __cdecl, void, Roblox::lua_State*, int);
	NEW_SUBROUTINE(lua_tolstring, 0x1435350, __stdcall, const char*, Roblox::lua_State*, int, unsigned int*);
	NEW_SUBROUTINE(lua_toboolean, 0x1435280, __cdecl, int, Roblox::lua_State*, int);
	NEW_SUBROUTINE(o_lua_tonumber, 0x1435480, __cdecl, double, Roblox::lua_State*, int, int);
	NEW_SUBROUTINE(lua_type, 0x1435800, __cdecl, int, Roblox::lua_State*, int);
	NEW_SUBROUTINE(lua_settop, 0x1435150, __cdecl, void, Roblox::lua_State*, int);
	NEW_SUBROUTINE(lua_insert, 0x1433C50, __cdecl, void, Roblox::lua_State*, int);
	NEW_SUBROUTINE(lua_settable, 0x14350D0, __cdecl, void, Roblox::lua_State*, int);
	NEW_SUBROUTINE(lua_createtable, 0x14336C0, __cdecl, int, Roblox::lua_State*, int, int);
	NEW_SUBROUTINE(lua_rawseti, 0x1434B00, __cdecl, int, Roblox::lua_State*, int, int);
	NEW_SUBROUTINE(luaL_checktype, 0x1435800, __cdecl, int, Roblox::lua_State*, int, int);
	NEW_SUBROUTINE(luaL_error, 0x1435D80, __cdecl, int, Roblox::lua_State*, const char*);
	NEW_SUBROUTINE(lua_setreadonly, 0x1434FF0, __cdecl, int, Roblox::lua_State*, int, bool);
	NEW_SUBROUTINE(lua_iscfunction, 0x1433D20, __cdecl, int, Roblox::lua_State*, int);

	NEW_SUBROUTINE(o_lua_getmetatable, 0x1433A70, __stdcall, int, Roblox::lua_State*, int);
	NEW_ADDRESS(r0_lua_getmetatable, 0x1433AA8);

	NEW_SUBROUTINE(o_lua_xmove, 0x14358B0, __cdecl, void, Roblox::lua_State*, Roblox::lua_State*, int);
	NEW_ADDRESS(r0_lua_xmove, 0x1435909);

	constexpr std::uint8_t RLUA_TNIL = 0;
	constexpr std::uint8_t RLUA_TLIGHTUSERDATA = 2;
	constexpr std::uint8_t RLUA_TNUMBER = 3;
	constexpr std::uint8_t RLUA_TBOOLEAN = 1;
	constexpr std::uint8_t RLUA_TSTRING = 5;
	constexpr std::uint8_t RLUA_TTHREAD = 9;
	constexpr std::uint8_t RLUA_TFUNCTION = 6;
	constexpr std::uint8_t RLUA_TTABLE = 8;
	constexpr std::uint8_t RLUA_TUSERDATA = 7;

	/* RLua macros & functions */
#define rlua_tonumber(ls, idx) Roblox::o_lua_tonumber(ls, idx, 0)
#define rlua_getmetatable(ls, idx) (Amp::Utils::SetByte(Roblox::r0_lua_getmetatable, 0x75), Roblox::o_lua_getmetatable(ls, idx), Amp::Utils::SetByte(Roblox::r0_lua_getmetatable, 0x74))
#define rlua_xmove(ls0, ls1, idx) (Amp::Utils::SetByte(Roblox::r0_lua_xmove, 0xEB), Roblox::o_lua_xmove(ls0, ls1, idx), Amp::Utils::SetByte(Roblox::r0_lua_xmove, 0x72))
#define rlua_getglobal(ls, k) Roblox::lua_getfield(ls, -10002, k)
#define rlua_setglobal(ls, k) Roblox::lua_setfield(ls, -10002, k)
#define rlua_pushstring(ls, str) Roblox::lua_pushlstring(ls, str, strlen(str))
#define rlua_tostring(ls, idx) Roblox::lua_tolstring(ls, idx, 0)
#define rlua_pop(ls, idx) Roblox::lua_settop(ls, -(idx-1))
#define rlua_pushcclosure(ls, f, uv) Roblox::o_lua_pushcclosure(ls, reinterpret_cast<Roblox::lua_CFunction>(Amp::Callcheck::GenerateSafeAddress(reinterpret_cast<unsigned int>(f))), NULL, uv, NULL)
#define rlua_pushcfunction(ls, f) rlua_pushcclosure(ls, f, 0)
#define rlua_register(L,n,f) (rlua_pushcfunction(L, f), rlua_setglobal(L, n))
#define rlua_clearstack(ls) Roblox::lua_settop(ls, 0)
}
/*-- EVERYTHING TO UPDATE -- END --*/