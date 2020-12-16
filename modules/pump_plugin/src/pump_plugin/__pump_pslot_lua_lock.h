#ifndef __PUMP_PLUGIN_LUA_LOCK_H
#define __PUMP_PLUGIN_LUA_LOCK_H
#if (PUMP_PLUGIN_LUA_MODE == 1)
#include "pump_core/pump_core_mutex.h"

::Pump::Core::Thread::CMutex g_luaLocker;

#define lua_lock(L) g_luaLocker.lock();
#endif // PUMP_PLUGIN_LUA_MODE

#endif // __PUMP_PLUGIN_LUA_LOCK_H