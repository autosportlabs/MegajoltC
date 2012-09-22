#include "FreeRTOS.h"
#include "task.h"
#include "luaBinding.h"
#include "luaTask.h"
#include "luaScript.h"
#include "FreeRTOS.h"
#include "usb_comm.h"
#include "string.h"
#include "memory.h"
#include "base64.h"
#include "lib_AT91SAM7S256.h"


void registerExtendedLuaFunctions(lua_State *L){

	//IO
	lua_register(L,"setPort",Lua_SetPort);
	lua_register(L,"getPort",Lua_GetPort);
	lua_register(L,"setPortDir",Lua_SetPortDirection);

	lua_register(L,"readADC",Lua_ReadADC);
	lua_register(L,"enableADC",Lua_EnableADC);
	lua_register(L,"sleep", Lua_Sleep);

}

int Lua_SetPort(lua_State *L){
	if (lua_gettop(L) >= 2){
		unsigned int port = lua_tointeger(L,1);
		unsigned int state = lua_tointeger(L,2);
		if (port >=0 && port <= 31){
			port = (1 << port);
			if (0 == state){
				AT91F_PIO_ClearOutput(AT91C_BASE_PIOA,port);
			}
			else{
				AT91F_PIO_SetOutput(AT91C_BASE_PIOA,port);
			}
		}
	}
	return 0;
}

int Lua_GetPort(lua_State *L){
	if (lua_gettop(L) >= 1){
		unsigned int port = lua_tointeger(L,1);
		if (port >=0 && port <= 31){
			port = (1 << port);
			unsigned int portState;
			portState = AT91F_PIO_GetInput(AT91C_BASE_PIOA) & port;
			//lua_pushinteger(L,portState != 0);
			lua_pushinteger(L,portState);
		}
		else{
			return 0;
		}
	}
	return 1;
}

int Lua_SetPortDirection(lua_State *L){

	if (lua_gettop(L) >= 2){
		unsigned int port = lua_tointeger(L,1);
		unsigned int dir = lua_tointeger(L,2);
		if (port >=0 && port <= 31){
			port = (1 << port);
			if (dir == 1){
				AT91F_PIO_CfgOutput(AT91C_BASE_PIOA,port);
			}
			else{
				AT91F_PIO_CfgInput(AT91C_BASE_PIOA, port);
				AT91C_BASE_PIOA->PIO_PPUER = port; //enable pullup
				AT91C_BASE_PIOA->PIO_IFER = port; //enable input filter
			}
		}
	}
	return 0;
}

int Lua_ReadADC(lua_State *L){
	if (lua_gettop(L) >= 1){
		unsigned int channel = lua_tointeger(L,1);

		AT91F_ADC_StartConversion (AT91C_BASE_ADC);
		/* Fourth Step: Waiting Stop Of conversion by pulling */
		while (!((AT91F_ADC_GetStatus (AT91C_BASE_ADC)) & (1<<channel)));

		/* Fifth Step: Read the ADC result output */
		unsigned int result = AT91F_ADC_GetLastConvertedData(AT91C_BASE_ADC);
		lua_pushinteger(L,result);
		return 1;
	}
	return 0;
}

int Lua_EnableADC(lua_State *L){
	if (lua_gettop(L) >=1){
		unsigned int channel = lua_tointeger(L,1);
		if (channel >=0 && channel <=7){
			AT91F_ADC_EnableChannel(AT91C_BASE_ADC,(1 << channel));
		}
	}
	return 0;
}

int Lua_Sleep(lua_State *L){
	if (lua_gettop(L) >= 1){
		vTaskDelay( lua_tointeger(L,1));
	}
}
