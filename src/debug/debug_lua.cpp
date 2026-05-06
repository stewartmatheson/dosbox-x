#include "dosbox.h"

#if C_DEBUG && C_LUA

#include <cstring>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "paging.h"
#include "logging.h"
#include "debug_lua.h"

static lua_State* L = nullptr;
static const uint64_t lua_mem_no_address = (~0ULL);

// ── dosbox.print ──────────────────────────────────────────────────────────

static int l_print(lua_State* ls) {
	const char* msg = luaL_checkstring(ls, 1);
	DEBUG_ShowMsg("LUA: %s\n", msg);
	return 0;
}

// ── Memory read (linear address) ──────────────────────────────────────────

static int l_readb(lua_State* ls) {
	uint32_t addr = (uint32_t)luaL_checkinteger(ls, 1);
	uint8_t val;
	if (mem_readb_checked((PhysPt)addr, &val)) {
		lua_pushnil(ls);
	} else {
		lua_pushinteger(ls, val);
	}
	return 1;
}

static int l_readw(lua_State* ls) {
	uint32_t addr = (uint32_t)luaL_checkinteger(ls, 1);
	uint16_t val;
	if (mem_readw_checked((PhysPt)addr, &val)) {
		lua_pushnil(ls);
	} else {
		lua_pushinteger(ls, val);
	}
	return 1;
}

static int l_readd(lua_State* ls) {
	uint32_t addr = (uint32_t)luaL_checkinteger(ls, 1);
	uint32_t val;
	if (mem_readd_checked((PhysPt)addr, &val)) {
		lua_pushnil(ls);
	} else {
		lua_pushinteger(ls, val);
	}
	return 1;
}

// ── Memory write (linear address) ─────────────────────────────────────────

static int l_writeb(lua_State* ls) {
	uint32_t addr = (uint32_t)luaL_checkinteger(ls, 1);
	uint8_t val = (uint8_t)luaL_checkinteger(ls, 2);
	if (mem_writeb_checked((PhysPt)addr, val)) {
		lua_pushboolean(ls, 0);
	} else {
		lua_pushboolean(ls, 1);
	}
	return 1;
}

static int l_writew(lua_State* ls) {
	uint32_t addr = (uint32_t)luaL_checkinteger(ls, 1);
	uint16_t val = (uint16_t)luaL_checkinteger(ls, 2);
	if (mem_writew_checked((PhysPt)addr, val)) {
		lua_pushboolean(ls, 0);
	} else {
		lua_pushboolean(ls, 1);
	}
	return 1;
}

static int l_writed(lua_State* ls) {
	uint32_t addr = (uint32_t)luaL_checkinteger(ls, 1);
	uint32_t val = (uint32_t)luaL_checkinteger(ls, 2);
	if (mem_writed_checked((PhysPt)addr, val)) {
		lua_pushboolean(ls, 0);
	} else {
		lua_pushboolean(ls, 1);
	}
	return 1;
}

// ── Memory read (seg:offset) ──────────────────────────────────────────────

static int l_readb_seg(lua_State* ls) {
	uint16_t seg = (uint16_t)luaL_checkinteger(ls, 1);
	uint32_t ofs = (uint32_t)luaL_checkinteger(ls, 2);
	uint64_t addr = GetAddress(seg, ofs);
	if (addr == lua_mem_no_address) { lua_pushnil(ls); return 1; }
	uint8_t val;
	if (mem_readb_checked((PhysPt)(uint32_t)addr, &val)) {
		lua_pushnil(ls);
	} else {
		lua_pushinteger(ls, val);
	}
	return 1;
}

static int l_readw_seg(lua_State* ls) {
	uint16_t seg = (uint16_t)luaL_checkinteger(ls, 1);
	uint32_t ofs = (uint32_t)luaL_checkinteger(ls, 2);
	uint64_t addr = GetAddress(seg, ofs);
	if (addr == lua_mem_no_address) { lua_pushnil(ls); return 1; }
	uint16_t val;
	if (mem_readw_checked((PhysPt)(uint32_t)addr, &val)) {
		lua_pushnil(ls);
	} else {
		lua_pushinteger(ls, val);
	}
	return 1;
}

static int l_readd_seg(lua_State* ls) {
	uint16_t seg = (uint16_t)luaL_checkinteger(ls, 1);
	uint32_t ofs = (uint32_t)luaL_checkinteger(ls, 2);
	uint64_t addr = GetAddress(seg, ofs);
	if (addr == lua_mem_no_address) { lua_pushnil(ls); return 1; }
	uint32_t val;
	if (mem_readd_checked((PhysPt)(uint32_t)addr, &val)) {
		lua_pushnil(ls);
	} else {
		lua_pushinteger(ls, val);
	}
	return 1;
}

// ── Memory write (seg:offset) ─────────────────────────────────────────────

static int l_writeb_seg(lua_State* ls) {
	uint16_t seg = (uint16_t)luaL_checkinteger(ls, 1);
	uint32_t ofs = (uint32_t)luaL_checkinteger(ls, 2);
	uint8_t val = (uint8_t)luaL_checkinteger(ls, 3);
	uint64_t addr = GetAddress(seg, ofs);
	if (addr == lua_mem_no_address) { lua_pushboolean(ls, 0); return 1; }
	lua_pushboolean(ls, !mem_writeb_checked((PhysPt)(uint32_t)addr, val));
	return 1;
}

static int l_writew_seg(lua_State* ls) {
	uint16_t seg = (uint16_t)luaL_checkinteger(ls, 1);
	uint32_t ofs = (uint32_t)luaL_checkinteger(ls, 2);
	uint16_t val = (uint16_t)luaL_checkinteger(ls, 3);
	uint64_t addr = GetAddress(seg, ofs);
	if (addr == lua_mem_no_address) { lua_pushboolean(ls, 0); return 1; }
	lua_pushboolean(ls, !mem_writew_checked((PhysPt)(uint32_t)addr, val));
	return 1;
}

static int l_writed_seg(lua_State* ls) {
	uint16_t seg = (uint16_t)luaL_checkinteger(ls, 1);
	uint32_t ofs = (uint32_t)luaL_checkinteger(ls, 2);
	uint32_t val = (uint32_t)luaL_checkinteger(ls, 3);
	uint64_t addr = GetAddress(seg, ofs);
	if (addr == lua_mem_no_address) { lua_pushboolean(ls, 0); return 1; }
	lua_pushboolean(ls, !mem_writed_checked((PhysPt)(uint32_t)addr, val));
	return 1;
}

// ── Read string from memory ───────────────────────────────────────────────

static int l_readstring(lua_State* ls) {
	uint32_t addr = (uint32_t)luaL_checkinteger(ls, 1);
	uint32_t maxlen = (uint32_t)luaL_optinteger(ls, 2, 256);
	luaL_Buffer buf;
	luaL_buffinit(ls, &buf);
	for (uint32_t i = 0; i < maxlen; i++) {
		uint8_t val;
		if (mem_readb_checked((PhysPt)(addr + i), &val) || val == 0) break;
		luaL_addchar(&buf, (char)val);
	}
	luaL_pushresult(&buf);
	return 1;
}

// ── Address resolution ────────────────────────────────────────────────────

static int l_linear(lua_State* ls) {
	uint16_t seg = (uint16_t)luaL_checkinteger(ls, 1);
	uint32_t ofs = (uint32_t)luaL_checkinteger(ls, 2);
	uint64_t addr = GetAddress(seg, ofs);
	if (addr == lua_mem_no_address) {
		lua_pushnil(ls);
	} else {
		lua_pushinteger(ls, (lua_Integer)(uint32_t)addr);
	}
	return 1;
}

// ── Memory search ─────────────────────────────────────────────────────────

static int l_memsearch(lua_State* ls) {
	uint32_t start = (uint32_t)luaL_checkinteger(ls, 1);
	uint32_t range = (uint32_t)luaL_checkinteger(ls, 2);
	size_t needle_len;
	const char* needle = luaL_checklstring(ls, 3, &needle_len);

	uint32_t end = start + range;
	if (end < start) end = 0xFFFFFFFFu;
	if (needle_len > (end - start)) {
		lua_newtable(ls);
		return 1;
	}
	uint32_t search_end = end - (uint32_t)needle_len + 1;

	lua_newtable(ls);
	int idx = 1;
	for (uint32_t addr = start; addr < search_end; addr++) {
		bool match = true;
		for (size_t j = 0; j < needle_len; j++) {
			uint8_t membyte;
			if (mem_readb_checked((PhysPt)(addr + (uint32_t)j), &membyte) ||
				(char)membyte != needle[j]) {
				match = false;
				break;
			}
		}
		if (match) {
			lua_pushinteger(ls, addr);
			lua_rawseti(ls, -2, idx++);
		}
	}
	return 1;
}

// ── Library registration ──────────────────────────────────────────────────

static const luaL_Reg dosbox_lib[] = {
	{"print",      l_print},
	{"readb",      l_readb},
	{"readw",      l_readw},
	{"readd",      l_readd},
	{"writeb",     l_writeb},
	{"writew",     l_writew},
	{"writed",     l_writed},
	{"sreadb",     l_readb_seg},
	{"sreadw",     l_readw_seg},
	{"sreadd",     l_readd_seg},
	{"swriteb",    l_writeb_seg},
	{"swritew",    l_writew_seg},
	{"swrited",    l_writed_seg},
	{"readstring", l_readstring},
	{"linear",     l_linear},
	{"memsearch",  l_memsearch},
	{nullptr,      nullptr}
};

static int luaopen_dosbox(lua_State* ls) {
	luaL_newlib(ls, dosbox_lib);
	return 1;
}

void DEBUG_LuaInit() {
	if (L) return;
	L = luaL_newstate();
	luaL_openlibs(L);
	luaL_requiref(L, "dosbox", luaopen_dosbox, 1);
	lua_pop(L, 1);
	DEBUG_ShowMsg("LUA: Lua %s initialized.\n", LUA_VERSION);
}

void DEBUG_LuaShutdown() {
	if (L) {
		lua_close(L);
		L = nullptr;
	}
}

static const char* recover_original_args(char* original_str, size_t args_len) {
	char* end = original_str + strlen(original_str);
	while (end > original_str && isspace(*(unsigned char*)(end - 1))) end--;
	return end - args_len;
}

bool DEBUG_LuaCommand(char* args, char* original_str) {
	if (!args || *args == '\0') {
		DEBUG_ShowMsg("LUA: Lua scripting commands.\n");
		DEBUG_ShowMsg("LUA EXEC <file>       - Execute a Lua script file.\n");
		DEBUG_ShowMsg("LUA EVAL <expression> - Evaluate a Lua expression.\n");
		DEBUG_ShowMsg("LUA RESET             - Reset the Lua state.\n");
		return true;
	}

	if (!L) DEBUG_LuaInit();

	if (strncmp(args, "EVAL ", 5) == 0) {
		const char* expr_upper = args + 5;
		size_t expr_len = strlen(expr_upper);
		const char* expr = recover_original_args(original_str, expr_len);
		if (luaL_dostring(L, expr) != LUA_OK) {
			DEBUG_ShowMsg("LUA ERROR: %s\n", lua_tostring(L, -1));
			lua_pop(L, 1);
		}
		return true;
	}

	if (strncmp(args, "EXEC ", 5) == 0) {
		const char* file_upper = args + 5;
		while (*file_upper == ' ') file_upper++;
		size_t file_len = strlen(file_upper);
		const char* file = recover_original_args(original_str, file_len);
		if (luaL_dofile(L, file) != LUA_OK) {
			DEBUG_ShowMsg("LUA ERROR: %s\n", lua_tostring(L, -1));
			lua_pop(L, 1);
		} else {
			DEBUG_ShowMsg("LUA: Executed %s\n", file);
		}
		return true;
	}

	if (strcmp(args, "RESET") == 0) {
		DEBUG_LuaShutdown();
		DEBUG_LuaInit();
		DEBUG_ShowMsg("LUA: State reset.\n");
		return true;
	}

	DEBUG_ShowMsg("LUA: Unknown sub-command. Type LUA for help.\n");
	return true;
}

#endif
