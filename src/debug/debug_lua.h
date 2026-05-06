#ifndef DEBUG_LUA_H
#define DEBUG_LUA_H

#include <cstdint>

#if C_DEBUG && C_LUA

extern uint64_t GetAddress(uint16_t seg, uint32_t offset);

void DEBUG_LuaInit();
void DEBUG_LuaShutdown();
bool DEBUG_LuaCommand(char* args, char* original_str);

#else

static inline void DEBUG_LuaInit() {}
static inline void DEBUG_LuaShutdown() {}
static inline bool DEBUG_LuaCommand(char*, char*) { return false; }

#endif

#endif
