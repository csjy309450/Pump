#if (PUMP_PLUGIN_LUA_MODE == 1)
#include <iostream>
#include "pump_plugin/__pump_plugin_lua_lock.h"
extern "C"
{
#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"
};
#include "SLB3/namespace.h"
#include "SLB3/internal/class.h"
#include "SLB3/implementation.h"
#include "pump_plugin/pump_pslot_lua_context.h"

namespace Pump
{
namespace Plugin
{

#define LUA() g_globalCtrl.L()
#define RUNLUA(script_) g_globalCtrl.RunLuaString(script_)

CLuaContext g_globalCtrl;

CLuaContext & GetGlobalLuaContext() {
    return g_globalCtrl;
}

CLuaContext::CLuaContext()
    : m_l(NULL)
{
    m_l = luaL_newstate();
    luaopen_base(m_l); //
    luaopen_table(m_l); //
    luaopen_package(m_l); //
    luaopen_io(m_l); //
    luaopen_string(m_l); //
    luaL_openlibs(m_l); //
}

CLuaContext::~CLuaContext()
{
    lua_close(m_l);
}

lua_State * CLuaContext::L()
{
    return m_l;
}

std::string CLuaContext::RunLuaScriptString(const std::string& script)
{
    const char * error = "";
    lua_State* L = lua_newthread(this->m_l);
    if (luaL_loadstring(this->m_l, script.c_str())
        || lua_pcall(this->m_l, 0, 0, 0))
    {
        error = lua_tostring(this->m_l, -1);
    }
    std::string msg = std::string(error);
    return msg;
}

void CLuaContext::RunLuaScriptObject(CLuaScript & script)
{
    const char * error = "";
    if (luaL_loadstring(this->m_l, script.GetIn().c_str())
        || lua_pcall(this->m_l, 0, 0, 0))
    {
        error = lua_tostring(this->m_l, -1);
    }
    script.SetOut(error);
}

std::string CLuaContext::RunLuaFunction(const std::string& script)
{
    lua_getglobal(m_l, script.c_str());
    lua_call(m_l, 0, 1);
    std::string error = lua_tostring(this->m_l, -1);
    return error;
}

std::string CLuaContext::RunLuaFunction(const std::string& script, int a)
{
    lua_getglobal(m_l, script.c_str());
    const char * ss = lua_tostring(m_l, -2);
    lua_pushnumber(m_l, a);
    
    try
    {
        lua_pcall(m_l, 1, 0, 0);
    }
    catch (std::exception & e)
    {
        printf("%s\n", e.what());
    }
    std::string error = lua_tostring(this->m_l, -1);
    lua_pop(m_l, 1);
    return error;
}

int CLuaContext::LoadLuaScriptFile(const char* szPath)
{
    int bRet = luaL_loadfile(this->m_l, szPath);
    if (bRet)
    {
        std::cout << "load test.lua file failed" << std::endl;
        return -1;
    }
    bRet = lua_pcall(this->m_l, 0, 0, 0);
    if (bRet)
    {
        std::cout << "call test.lua file failed" << std::endl;
        return -1;
    }
    return 0;
}

}
}

SLB3_IMPLEMENTATION(::Pump::Core::Thread::CMutex, cdec) {
    cdec
        .constructor<void>()
        .set("Lock", &::Pump::Core::Thread::CMutex::Lock)
        .set("TryLock", &::Pump::Core::Thread::CMutex::TryLock)
        .set("Unlock", &::Pump::Core::Thread::CMutex::Unlock)
        ;
}

SLB3_IMPLEMENTATION(::Pump::Core::Thread::CThread, cdec) {
    cdec
        .set("Start", &::Pump::Core::Thread::CThread::Start)
        .set("Stop", &::Pump::Core::Thread::CThread::Stop)
        .set("SetData", &::Pump::Core::Thread::CThread::SetData)
        ;
}

SLB3_IMPLEMENTATION(::Pump::Slot::CLuaThreadData, cdec) {
    cdec
        .constructor<void>()
        .set("SetLuaScript", &::Pump::Slot::CLuaThreadData::SetLuaScript)
        //.set("SetCallback", &::Pump::Slot::CLuaThreadData::SetCallback)
        .set("GetLuaScript", &::Pump::Slot::CLuaThreadData::GetLuaScript)
        .set("This", &::Pump::Slot::CLuaThreadData::This)
        //.set("GetCallback", &::Pump::Slot::CLuaThreadData::GetCallback)
        ;
}

SLB3_IMPLEMENTATION(::Pump::Slot::CLuaThread, cdec) {
    cdec
        .constructor<void>()
        .inherits<::Pump::Core::Thread::CThread>()
        .set("This", &::Pump::Slot::CLuaThread::This)
        ;
}
#endif // PUMP_PLUGIN_LUA_MODE