#ifndef LUABINDING_H_
#define LUABINDING_H_

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

void registerExtendedLuaFunctions(lua_State *L);

int Lua_SetPort(lua_State *L);
int Lua_GetPort(lua_State *L);
int Lua_SetPortDirection(lua_State *L);

int Lua_ReadADC(lua_State *L);
int Lua_EnableADC(lua_State *L);
int Lua_Sleep(lua_State *L);





#endif

