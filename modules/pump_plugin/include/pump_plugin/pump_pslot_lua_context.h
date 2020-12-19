#ifndef PUMP_PLUGIN_LUA_CONTEXT_H
#define PUMP_PLUGIN_LUA_CONTEXT_H
#if (PUMP_PLUGIN_LUA_MODE == 1)
#include <string>
extern "C"
{
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
};
#include "SLB3/namespace.h"
#include "SLB3/internal/class.h"
#include "SLB3/implementation.h"
#include "pump_core/pump_core_nonop.h"
#include "pump_core/pump_core_thread.h"
#include "pump_core/pump_core_mutex.h"

struct lua_State;

namespace Pump
{
namespace Plugin
{

class CLuaContext;

CLuaContext & GetGlobalLuaContext();

class CLuaScript
{
public:
    CLuaScript() {}
    CLuaScript(const std::string & strIn) : m_strIn(strIn) {}

    virtual ~CLuaScript() {}

    CLuaScript(const CLuaScript & other)
        : m_strIn(other.m_strIn)
        , m_strOut(other.m_strOut)
    {
    }

    void SetIn(const std::string & strIn)
    {
        m_strIn = strIn;
    }

    const std::string & GetIn() const
    {
        return m_strIn;
    }

    void SetOut(const std::string & strOut)
    {
        m_strOut = strOut;
    }

    const std::string & GetOut() const
    {
        return m_strOut;
    }
protected:
    std::string m_strIn;
    std::string m_strOut;
};

class CLSLoadEntry : public CLuaScript
{
public:
    CLSLoadEntry(const std::string & name, const std::string & version)
        : CLuaScript(\
        "SLB." + name + "=dofile(\"E:/VMware/YZ/Pump/modules/pump_plugin/test/" + name + "@" + version + "/script/entry.lua\");" \
        "print(" + name + ");")
    {
    }

    virtual ~CLSLoadEntry() {}
};

class CLuaContext 
    : public ::Pump::Core::CNonCopyOperator
{
public:
    CLuaContext();
    virtual ~CLuaContext();

    lua_State *L();

    std::string RunLuaScriptString(const std::string& script);
    void RunLuaScriptObject(CLuaScript & script);
    std::string RunLuaFunction(const std::string& script);
    std::string RunLuaFunction(const std::string& script, int a);
    int LoadLuaScriptFile(const char* szPath);
private:
    CLuaContext(const CLuaContext &);

private:
    lua_State *m_l;
};

typedef pump_void_t * (*ThreadCallback)(pump_void_t *);

class CLuaThreadData
{
public:
    CLuaThreadData() : m_pThreadCallback(NULL){}
    ~CLuaThreadData() {}
    void SetLuaScript(const char* szScript)
    {
        m_strLuaScript = szScript;
    }
    void SetCallback(pump_void_t * (*pThreadCallback)(pump_void_t *))
    {
        m_pThreadCallback = pThreadCallback;
    }
    const char* GetLuaScript() const { return m_strLuaScript.c_str(); }
    ThreadCallback GetCallback() const { return m_pThreadCallback; }
    CLuaThreadData * This() { return this; }
private:
    std::string m_strLuaScript;
    ThreadCallback m_pThreadCallback;
};

class CLuaThread : public ::Pump::Core::Thread::CThread
{
public:
    CLuaThread * This() { return this; }
private:
    virtual pump_void_t * ThreadCallback(pump_void_t * pData)
    {
        if (!pData)
        {
            printf("error thread data.\n");
            return NULL;
        }
        CLuaThreadData * pThxData = static_cast<CLuaThreadData *>(pData);
        if (pThxData->GetCallback())
        {
            pThxData->GetCallback()(NULL);
        }
        else if (pThxData->GetLuaScript() != 0)
        {
            try
            {
                printf("%s\n", GetGlobalLuaContext().RunLuaScriptString(pThxData->GetLuaScript()).c_str());
                //printf("%s\n", GetGlobalLuaContext().RunLuaFunction("test_plugin:__OnMainCore();").c_str());
            }
            catch (std::exception& e)
            {
                printf("%s\n", e.what());
            }
        }
        return NULL;
    }
};

}
}

SLB3_DECLARATION_EX(::Pump::Core::Thread::CMutex, "SLB.CMutex", policy::Default, )
SLB3_DECLARATION_EX(::Pump::Core::Thread::CThread, "SLB.CThread", policy::Default, )
SLB3_DECLARATION_EX(::Pump::Slot::CLuaThreadData, "SLB.CLuaThreadData", policy::Default, )
SLB3_DECLARATION_EX(::Pump::Slot::CLuaThread, "SLB.CLuaThread", policy::Default, )
#endif // PUMP_PLUGIN_LUA_MODE

#endif // PUMP_PLUGIN_LUA_CONTEXT_H
