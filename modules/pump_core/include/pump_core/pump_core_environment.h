/**
 * @file pump_core_environment.h
 * @brief Library Header.
 * @details Declaration of environment variable control interface.
 *
 * @author yangzheng [263693992@qq.com]
 * @date 2020-08-11
 * @version v0.9.0
 * @copyright Copyright (c) 2020 yangzheng
 *
 * @par Change History
 * <table>
 * <tr><th>Date<th>Version<th>Author<th>Description
 * <tr><td>2020-08-11<td>v0.9.0<td>yangzheng<td>create file.
 * </table>
 */

#ifndef PUMP_CORE_ENVIRONMENT_H
#define PUMP_CORE_ENVIRONMENT_H
#include "pump_macro/pump_pre.h"
#include <string>
#include <map>
#include <set>
#include "pump_core/pump_core_config.h"
#include "pump_core/pump_core_types.h"

namespace Pump
{
namespace Core
{

typedef std::set<std::string> EnvVarItemType;
typedef std::pair<std::string, EnvVarItemType> EnvVarContainerItemType;
typedef std::map<std::string, EnvVarItemType> EnvVarContainerType;

class PUMP_CORE_CLASS CEnvVarSet
{
public:
    friend class CEnvironment;
    typedef EnvVarContainerType::iterator IteratorType;
    typedef EnvVarContainerType::const_iterator ConstIteratorType;
public:
    CEnvVarSet() {}
    CEnvVarSet(const CEnvVarSet & other)
        : m_envContainer(other.m_envContainer)
    {}
    CEnvVarSet & operator=(const CEnvVarSet & other)
    {
        m_envContainer = (other.m_envContainer);
        return *this;
    }

    void Insert(const std::string & strKey)
    {
        EnvVarContainerType::iterator it = m_envContainer.find(strKey);
        if (it == m_envContainer.end())
        {
            EnvVarItemType varItem;
            m_envContainer.insert(EnvVarContainerItemType(strKey, varItem));
        }
    }
    void Insert(const std::string & strKey, const std::string & strVar)
    {
        EnvVarContainerType::iterator it = m_envContainer.find(strKey);
        if (it == m_envContainer.end())
        {
            EnvVarItemType varItem;
            varItem.insert(strVar);
            m_envContainer.insert(EnvVarContainerItemType(strKey, varItem));
        }
        else
        {
            it->second.insert(strVar);
        }
    }

    void Insert(const std::string & strKey, const EnvVarItemType & var)
    {
        EnvVarContainerType::iterator it = m_envContainer.find(strKey);
        if (it == m_envContainer.end())
        {
            m_envContainer.insert(EnvVarContainerItemType(strKey, var));
        }
        else
        {
            it->second = var;
        }
    }

    void Insert(const EnvVarContainerItemType & var)
    {
        m_envContainer.insert(var);
    }

    void Remove(const std::string & strKey, const std::string & strVar)
    {
        EnvVarContainerType::iterator it = m_envContainer.find(strKey);
        if (it != m_envContainer.end())
        {
            it->second.erase(strVar);
        }
    }

    void Remove(const std::string & strKey)
    {
        EnvVarContainerType::iterator it = m_envContainer.find(strKey);
        m_envContainer.erase(it);
    }

    IteratorType Find(const std::string & strKey)
    {
        IteratorType it = m_envContainer.find(strKey);
        return it;
    }

    IteratorType Begin()
    {
        return m_envContainer.begin();
    }


    ConstIteratorType CBegin() const
    {
#if (defined PUMP_HAS_CPP11)
        return m_envContainer.cbegin();
#else 
        return m_envContainer.begin();
#endif  // (defined PUMP_HAS_CPP11)
    }

    IteratorType End()
    {
        return m_envContainer.end();
    }

    ConstIteratorType CEnd() const
    {
#if (defined PUMP_HAS_CPP11)
        return m_envContainer.cend();
#else 
        return m_envContainer.end();
#endif  // (defined PUMP_HAS_CPP11)
    }

    EnvVarContainerType::size_type Size() const
    {
        return m_envContainer.size();
    }

    std::string ToString(const std::string & strKey) const
    {
        ConstIteratorType envItem = m_envContainer.find(strKey);
        std::string strOut;
        for (EnvVarItemType::const_iterator it = envItem->second.begin();;)
        {
            strOut += (*it);
            if ((++it) != envItem->second.end())
            {
                strOut += ";";
            }
            else
            {
                strOut += ";";
                break;
            }
        }
        return strOut;
    }

    void Clear()
    {
        m_envContainer.clear();
    }
private:
    EnvVarContainerType m_envContainer;
};

class PUMP_CORE_CLASS CEnvironment
{
public:
    CEnvironment();
    ~CEnvironment();
    pump_bool_t GetEnvStrings();
    pump_void_t FreeEnvStrings();
    pump_bool_t SetEnvVariable(
        const pump_char_t * pName
        , const pump_char_t *  pValue);
    pump_bool_t GetEnvVariable(
        const pump_char_t * pName
        , pump_char_t * pValue
        , pump_uint32_t dwLen);
    const pump_char_t * GetFirstString();
    const pump_char_t * GetNextString();

    pump_void_t GetForEnvVarSet(CEnvVarSet & refEvnVarSet)
    {
#ifdef PUMP_OS_WINDOWS
        this->GetEnvStrings();
        pump_char_t * pEvnVar = (pump_char_t *)this->GetFirstString();
        if (!pEvnVar)
        {
            return;
        }
        refEvnVarSet.Clear();
        std::string strEvnVar, strKey, strValue, strVarItem;
        while (pEvnVar)
        {
            strEvnVar = pEvnVar;
            std::string::size_type pos = strEvnVar.find("=")
                , subPos = 0;
            if (pos == std::string::npos)
            {
                pEvnVar = (pump_char_t *)this->GetNextString();
                continue;
            }
            if (pos == 0 && strEvnVar[3] == '=')
            {
                pEvnVar = (pump_char_t *)this->GetNextString();
                continue;
            }
            strKey = strEvnVar.substr(0, pos);
            strValue = strEvnVar.substr(pos + 1, strEvnVar.size() - (pos - 1));
            EnvVarItemType envItem;
            pos = 0;
            do
            {
                subPos = strValue.find(";", pos);
                if (subPos == std::string::npos)
                {
                    envItem.insert(strValue);
                    break;
                }
                strVarItem = strValue.substr(pos, subPos - pos);
                envItem.insert(strVarItem);
                pos = subPos + 1;
            } while (1);
            refEvnVarSet.Insert(strKey, envItem);
            pEvnVar = (pump_char_t *)this->GetNextString();
        }
#endif // PUMP_OS_WINDOWS
    }
    pump_void_t SetFromEnvVarSet(const CEnvVarSet & refEvnVarSet)
    {
        std::string strEnvVar;
        pump_bool_t ret = PUMP_FALSE;
        for (CEnvVarSet::ConstIteratorType it = refEvnVarSet.CBegin(); it != refEvnVarSet.CEnd(); ++it)
        {
            strEnvVar = refEvnVarSet.ToString(it->first);
            ret = this->SetEnvVariable(it->first.c_str(), strEnvVar.c_str());
        }
    }
    
private:
#ifdef PUMP_OS_WINDOWS
    pump_char_t * m_pEnvStrings;
    pump_char_t * m_pEnvStringsTmp;
#elif (defined PUMP_OS_POSIX)
    pump_char_t ** m_pEnvStrings;
    pump_char_t ** m_pEnvStringsTmp;
#endif // PUMP_OS_WINDOWS
};

}
}

#endif // PUMP_CORE_ENVIRONMENT_H


