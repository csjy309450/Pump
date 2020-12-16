/**
 * @file pump_core_cmdparse.hpp
 * @brief Library Header.
 * @details Declaration of exe console parameter interface.
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

#ifndef PUMP_CORE_CMDPARSE_HPP
#define PUMP_CORE_CMDPARSE_HPP
#include "pump_macro/pump_pre.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <typeinfo>
#include <cstring>
#include <algorithm>
#include <cstdlib>
#include "pump_core/os_wrapper/pump_core_os_api.h"

namespace Pump
{
namespace Core
{
namespace Cmder
{

template <typename Target, typename Source, bool bSame>
class CLexicalCast
{
public:
    static Target cast(const Source &arg){
        Target ret;
        std::stringstream ss;
        if (!(ss << arg && ss >> ret && ss.eof()))
            throw std::bad_cast();
        return ret;
    }
};

template <typename Target, typename Source>
class CLexicalCast<Target, Source, true>
{
public:
    static Target cast(const Source &arg){
        return arg;
    }
};

template <typename Source>
class CLexicalCast<std::string, Source, false>
{
public:
    static std::string cast(const Source &arg){
        std::ostringstream ss;
        ss << arg;
        return ss.str();
    }
};

template <typename Target>
class CLexicalCast<Target, std::string, false>
{
public:
    static Target cast(const std::string &arg)
    {
        Target ret;
        std::istringstream ss(arg);
        if (!(ss >> ret && ss.eof()))
            throw std::bad_cast();
        return ret;
    }
};

template <typename T1, typename T2>
class CTheSame
{
public:
    static const bool value = false;
};

template <typename T>
class CTheSame<T, T>
{
public:
    static const bool value = true;
};

template<typename Target, typename Source>
Target __IexicalCast(const Source &arg)
{
    return CLexicalCast<Target, Source, CTheSame<Target, Source>::value>::cast(arg);
}

//static inline std::string demangle(const std::string &name)
//{
//    int status = 0;
//    const char *p = name.c_str();//abi::__cxa_demangle(name.c_str(), 0, 0, &status);
//    std::string ret(p);
//    //free(p);
//    return ret;
//}

template <class T>
std::string __ReadableTypename()
{
    return "Unknown Type";
}

template <>
inline std::string __ReadableTypename<int>()
{
    return "int";
}

template <>
inline std::string __ReadableTypename<std::string>()
{
    return "string";
}

template <>
inline std::string __ReadableTypename<float>()
{
    return "float";
}

template <>
inline std::string __ReadableTypename<double>()
{
    return "double";
}

template <class T>
std::string __DefaultValue(T def)
{
    return __IexicalCast<std::string>(def);
}

class CCmdParseError : public std::exception
{
public:
    CCmdParseError(const std::string &msg) : msg(msg){}
    ~CCmdParseError() throw() {}
    const char *what() const throw() { return msg.c_str(); }
private:
    std::string msg;
};

template <class T>
struct CDefaultReader
{
    T operator()(const std::string &str){
        return __IexicalCast<T>(str);
    }
};

template <class T>
struct CRangeReader
{
    CRangeReader(const T &low, const T &high) : low(low), high(high) {}
    T operator()(const std::string &s) const {
        T ret = CDefaultReader<T>()(s);
        if (!(ret >= low && ret <= high)) throw CCmdParseError("range_error");
        return ret;
    }
private:
    T low, high;
};

template <class T>
CRangeReader<T> Range(const T &low, const T &high)
{
    return CRangeReader<T>(low, high);
}

template <class T>
struct CEnumReader
{
    T operator()(const std::string &s){
        T ret = CDefaultReader<T>()(s);
        if (std::find(alt.begin(), alt.end(), ret) == alt.end())
            throw CCmdParseError("");
        return ret;
    }
    void Add(const T &v){ alt.push_back(v); }
private:
    std::vector<T> alt;
};

template <class T>
CEnumReader<T> OneOfEnum(T a1)
{
    CEnumReader<T> ret;
    ret.Add(a1);
    return ret;
}

template <class T>
CEnumReader<T> OneOfEnum(T a1, T a2)
{
    CEnumReader<T> ret;
    ret.Add(a1);
    ret.Add(a2);
    return ret;
}

template <class T>
CEnumReader<T> OneOfEnum(T a1, T a2, T a3)
{
    CEnumReader<T> ret;
    ret.Add(a1);
    ret.Add(a2);
    ret.Add(a3);
    return ret;
}

template <class T>
CEnumReader<T> OneOfEnum(T a1, T a2, T a3, T a4)
{
    CEnumReader<T> ret;
    ret.Add(a1);
    ret.Add(a2);
    ret.Add(a3);
    ret.Add(a4);
    return ret;
}

template <class T>
CEnumReader<T> OneOfEnum(T a1, T a2, T a3, T a4, T a5)
{
    CEnumReader<T> ret;
    ret.Add(a1);
    ret.Add(a2);
    ret.Add(a3);
    ret.Add(a4);
    ret.Add(a5);
    return ret;
}

template <class T>
CEnumReader<T> OneOfEnum(T a1, T a2, T a3, T a4, T a5, T a6)
{
    CEnumReader<T> ret;
    ret.Add(a1);
    ret.Add(a2);
    ret.Add(a3);
    ret.Add(a4);
    ret.Add(a5);
    ret.Add(a6);
    return ret;
}

template <class T>
CEnumReader<T> OneOfEnum(T a1, T a2, T a3, T a4, T a5, T a6, T a7)
{
    CEnumReader<T> ret;
    ret.Add(a1);
    ret.Add(a2);
    ret.Add(a3);
    ret.Add(a4);
    ret.Add(a5);
    ret.Add(a6);
    ret.Add(a7);
    return ret;
}

template <class T>
CEnumReader<T> OneOfEnum(T a1, T a2, T a3, T a4, T a5, T a6, T a7, T a8)
{
    CEnumReader<T> ret;
    ret.Add(a1);
    ret.Add(a2);
    ret.Add(a3);
    ret.Add(a4);
    ret.Add(a5);
    ret.Add(a6);
    ret.Add(a7);
    ret.Add(a8);
    return ret;
}

template <class T>
CEnumReader<T> OneOfEnum(T a1, T a2, T a3, T a4, T a5, T a6, T a7, T a8, T a9)
{
    CEnumReader<T> ret;
    ret.Add(a1);
    ret.Add(a2);
    ret.Add(a3);
    ret.Add(a4);
    ret.Add(a5);
    ret.Add(a6);
    ret.Add(a7);
    ret.Add(a8);
    ret.Add(a9);
    return ret;
}

template <class T>
CEnumReader<T> OneOfEnum(T a1, T a2, T a3, T a4, T a5, T a6, T a7, T a8, T a9, T a10)
{
    CEnumReader<T> ret;
    ret.Add(a1);
    ret.Add(a2);
    ret.Add(a3);
    ret.Add(a4);
    ret.Add(a5);
    ret.Add(a6);
    ret.Add(a7);
    ret.Add(a8);
    ret.Add(a9);
    ret.Add(a10);
    return ret;
}

//-----

class CCmdParser
{
public:
    CCmdParser()
    {
    }
    ~CCmdParser()
    {
        for (std::map<std::string, COptionBase*>::iterator p = m_mapOptions.begin();
            p != m_mapOptions.end(); p++)
            delete p->second;
    }

    void Add(const std::string &name,
        char short_name = 0,
        const std::string &desc = "")
    {
        if (m_mapOptions.count(name)) throw CCmdParseError("multiple definition: " + name);
        m_mapOptions[name] = new __COptionWithoutValue(name, short_name, desc);
        m_vecOrdered.push_back(m_mapOptions[name]);
    }

    template <class T>
    void Add(const std::string &name,
        char short_name = 0,
        const std::string &desc = "",
        bool need = true,
        const T def = T())
    {
        Add(name, short_name, desc, need, def, CDefaultReader<T>());
    }

    template <class T, class F>
    void Add(const std::string &name,
        char short_name = 0,
        const std::string &desc = "",
        bool need = true,
        const T def = T(),
        F reader = F())
    {
        if (m_mapOptions.count(name)) throw CCmdParseError("multiple definition: " + name);
        m_mapOptions[name] = new __COptionWithValueAndReader<T, F>(name, short_name, need, def, desc, reader);
        m_vecOrdered.push_back(m_mapOptions[name]);
    }

    void Footer(const std::string &f)
    {
        m_strFtr = f;
    }

    void SetProgramName(const std::string &name)
    {
        m_strProgName = name;
    }

    bool Exist(const std::string &name) const
    {
        if (m_mapOptions.count(name) == 0) throw CCmdParseError("there is no flag: --" + name);
        return m_mapOptions.find(name)->second->HasSet();
    }

    template <class T>
    const T &Get(const std::string &name) const
    {
        if (m_mapOptions.count(name) == 0) throw CCmdParseError("there is no flag: --" + name);
        const __COptionWithValue<T> *p = dynamic_cast<const __COptionWithValue<T>*>(m_mapOptions.find(name)->second);
        if (p == NULL) throw CCmdParseError("type mismatch flag '" + name + "'");
        return p->Get();
    }

    const std::vector<std::string> &Rest() const
    {
        return m_vecOthers;
    }

    bool Parse(const std::string &arg)
    {
        std::vector<std::string> args;

        std::string buf;
        bool in_quote = false;
        for (std::string::size_type i = 0; i < arg.length(); i++){
            if (arg[i] == '\"'){
                in_quote = !in_quote;
                continue;
            }

            if (arg[i] == ' ' && !in_quote){
                args.push_back(buf);
                buf = "";
                continue;
            }

            if (arg[i] == '\\'){
                i++;
                if (i >= arg.length()){
                    m_vecErrors.push_back("unexpected occurrence of '\\' at end of string");
                    return false;
                }
            }

            buf += arg[i];
        }

        if (in_quote){
            m_vecErrors.push_back("quote is not closed");
            return false;
        }

        if (buf.length() > 0)
            args.push_back(buf);

        for (size_t i = 0; i < args.size(); i++)
            std::cout << "\"" << args[i] << "\"" << std::endl;

        return Parse(args);
    }

    bool Parse(const std::vector<std::string> &args)
    {
        int argc = static_cast<int>(args.size());
        std::vector<const char*> argv(argc);

        for (int i = 0; i < argc; i++)
            argv[i] = args[i].c_str();

        return Parse(argc, &argv[0]);
    }

    bool Parse(int argc, const char * const argv[])
    {
        m_vecErrors.clear();
        m_vecOthers.clear();

        if (argc < 1){
            m_vecErrors.push_back("argument number must be longer than 0");
            return false;
        }
        if (m_strProgName == "")
            m_strProgName = argv[0];

        std::map<char, std::string> lookup;
        for (std::map<std::string, COptionBase*>::iterator p = m_mapOptions.begin();
            p != m_mapOptions.end(); p++){
            if (p->first.length() == 0) continue;
            char initial = p->second->ShortName();
            if (initial){
                if (lookup.count(initial)>0){
                    lookup[initial] = "";
                    m_vecErrors.push_back(std::string("short option '") + initial + "' is ambiguous");
                    return false;
                }
                else lookup[initial] = p->first;
            }
        }

        for (int i = 1; i < argc; i++)
        {
            if (strncmp(argv[i], "--", 2) == 0){
                const char *p = strchr(argv[i] + 2, '=');
                if (p){
                    std::string name(argv[i] + 2, p);
                    std::string val(p + 1);
                    SetOption(name, val);
                }
                else{
                    std::string name(argv[i] + 2);
                    if (m_mapOptions.count(name) == 0){
                        m_vecErrors.push_back("undefined option: --" + name);
                        continue;
                    }
                    if (m_mapOptions[name]->HasValue()){
                        if (i + 1 >= argc){
                            m_vecErrors.push_back("option needs value: --" + name);
                            continue;
                        }
                        else{
                            i++;
                            SetOption(name, argv[i]);
                        }
                    }
                    else{
                        SetOption(name);
                    }
                }
            }
            else if (strncmp(argv[i], "-", 1) == 0){
                if (!argv[i][1]) continue;
                char last = argv[i][1];
                for (int j = 2; argv[i][j]; j++){
                    last = argv[i][j];
                    if (lookup.count(argv[i][j - 1]) == 0){
                        m_vecErrors.push_back(std::string("undefined short option: -") + argv[i][j - 1]);
                        continue;
                    }
                    if (lookup[argv[i][j - 1]] == ""){
                        m_vecErrors.push_back(std::string("ambiguous short option: -") + argv[i][j - 1]);
                        continue;
                    }
                    SetOption(lookup[argv[i][j - 1]]);
                }

                if (lookup.count(last) == 0){
                    m_vecErrors.push_back(std::string("undefined short option: -") + last);
                    continue;
                }
                if (lookup[last] == ""){
                    m_vecErrors.push_back(std::string("ambiguous short option: -") + last);
                    continue;
                }

                if (i + 1 < argc && m_mapOptions[lookup[last]]->HasValue()){
                    SetOption(lookup[last], argv[i + 1]);
                    i++;
                }
                else{
                    SetOption(lookup[last]);
                }
            }
            else{
                m_vecOthers.push_back(argv[i]);
            }
        }

        for (std::map<std::string, COptionBase*>::iterator p = m_mapOptions.begin();
            p != m_mapOptions.end(); p++)
            if (!p->second->Valid())
                m_vecErrors.push_back("need option: --" + std::string(p->first));

        return m_vecErrors.size() == 0;
    }

    void ParseCheck(const std::string &arg)
    {
        if (!m_mapOptions.count("help"))
            Add("help", '?', "print this message");
        Check(0, Parse(arg));
    }

    void ParseCheck(const std::vector<std::string> &args)
    {
        if (!m_mapOptions.count("help"))
            Add("help", '?', "print this message");
        Check(args.size(), Parse(args));
    }

    void ParseCheck(int argc, char *argv[])
    {
        if (!m_mapOptions.count("help"))
            Add("help", '?', "print this message");
        Check(argc, Parse(argc, argv));
    }

    std::string error() const
    {
        return m_vecErrors.size() > 0 ? m_vecErrors[0] : "";
    }

    std::string ErrorFull() const
    {
        std::ostringstream oss;
        for (size_t i = 0; i < m_vecErrors.size(); i++)
            oss << m_vecErrors[i] << std::endl;
        return oss.str();
    }

    std::string Usage() const
    {
        std::ostringstream oss;
        oss << "usage: " << m_strProgName << " ";
        for (size_t i = 0; i < m_vecOrdered.size(); i++){
            if (m_vecOrdered[i]->Must())
                oss << m_vecOrdered[i]->ShortDescription() << " ";
        }

        oss << "[options] ... " << m_strFtr << std::endl;
        oss << "options:" << std::endl;

        size_t max_width = 0;
        for (size_t i = 0; i < m_vecOrdered.size(); i++){
            max_width = PUMP_CORE_Max(max_width, m_vecOrdered[i]->Name().length());
        }
        for (size_t i = 0; i < m_vecOrdered.size(); i++){
            if (m_vecOrdered[i]->ShortName()){
                oss << "  -" << m_vecOrdered[i]->ShortName() << ", ";
            }
            else{
                oss << "      ";
            }

            oss << "--" << m_vecOrdered[i]->Name();
            for (size_t j = m_vecOrdered[i]->Name().length(); j < max_width + 4; j++)
                oss << ' ';
            oss << m_vecOrdered[i]->Description() << std::endl;
        }
        return oss.str();
    }

    class COptionBase
    {
    public:
        virtual ~COptionBase(){}

        virtual bool HasValue() const = 0;
        virtual bool Set() = 0;
        virtual bool Set(const std::string &value) = 0;
        virtual bool HasSet() const = 0;
        virtual bool Valid() const = 0;
        virtual bool Must() const = 0;

        virtual const std::string &Name() const = 0;
        virtual char ShortName() const = 0;
        virtual const std::string &Description() const = 0;
        virtual std::string ShortDescription() const = 0;
    };

    const std::string Help() const
    {
        std::string strHelp;
        for (std::map<std::string, CCmdParser::COptionBase*>::const_iterator it = m_mapOptions.begin();
            it != m_mapOptions.end(); ++it)
        {
            strHelp += it->second->Description() + '\n';
        }
        return strHelp;
    }

private:

    void Check(int argc, bool ok){
        if ((argc == 1 && m_mapOptions.size() > 0 && !ok) || Exist("help")){
            throw CCmdParseError(Usage());
        }

        if (!ok){
            throw CCmdParseError(Usage() + error());
        }
    }

    void SetOption(const std::string &name){
        if (m_mapOptions.count(name) == 0){
            m_vecErrors.push_back("undefined option: --" + name);
            return;
        }
        if (!m_mapOptions[name]->Set()){
            m_vecErrors.push_back("option needs value: --" + name);
            return;
        }
    }

    void SetOption(const std::string &name, const std::string &value){
        if (m_mapOptions.count(name) == 0){
            m_vecErrors.push_back("undefined option: --" + name);
            return;
        }
        if (!m_mapOptions[name]->Set(value)){
            m_vecErrors.push_back("option value is invalid: --" + name + "=" + value);
            return;
        }
    }

    class __COptionWithoutValue
        : public COptionBase
    {
    public:
        __COptionWithoutValue(const std::string &name,
            char short_name,
            const std::string &desc)
            :nam(name), snam(short_name), desc(desc), has(false){
        }
        ~__COptionWithoutValue(){}

        bool HasValue() const { return false; }

        bool Set(){
            has = true;
            return true;
        }

        bool Set(const std::string &){
            return false;
        }

        bool HasSet() const {
            return has;
        }

        bool Valid() const{
            return true;
        }

        bool Must() const{
            return false;
        }

        const std::string &Name() const{
            return nam;
        }

        char ShortName() const{
            return snam;
        }

        const std::string &Description() const {
            return desc;
        }

        std::string ShortDescription() const{
            return "--" + nam;
        }

    private:
        std::string nam;
        char snam;
        std::string desc;
        bool has;
    };

    template <typename T>
    class __COptionWithValue
        : public COptionBase
    {
    public:
        __COptionWithValue(const std::string &name,
            char short_name,
            bool need,
            const T &def,
            const std::string &desc)
            : nam(name), snam(short_name), need(need), has(false)
            , def(def), actual(def) {
            this->desc = FullDescription(desc);
        }
        ~__COptionWithValue(){}

        const T &Get() const {
            return actual;
        }

        bool HasValue() const { return true; }

        bool Set(){
            return false;
        }

        bool Set(const std::string &value){
            try{
                actual = Read(value);
                has = true;
            }
            catch (const std::exception &e){
                e;
                return false;
            }
            return true;
        }

        bool HasSet() const{
            return has;
        }

        bool Valid() const{
            if (need && !has) return false;
            return true;
        }

        bool Must() const{
            return need;
        }

        const std::string &Name() const{
            return nam;
        }

        char ShortName() const{
            return snam;
        }

        const std::string &Description() const {
            return desc;
        }

        std::string ShortDescription() const{
            return "--" + nam + "=" + __ReadableTypename<T>();
        }

    protected:
        std::string FullDescription(const std::string &desc){
            return
                desc + " (" + __ReadableTypename<T>() +
                (need ? "" : " [=" + __DefaultValue<T>(def)+"]")
                + ")";
        }

        virtual T Read(const std::string &s) = 0;

        std::string nam;
        char snam;
        bool need;
        std::string desc;

        bool has;
        T def;
        T actual;
    };

    template <typename T, typename F>
    class __COptionWithValueAndReader
        : public __COptionWithValue<T>
    {
    public:
        __COptionWithValueAndReader(const std::string &name,
            char short_name,
            bool need,
            const T def,
            const std::string &desc,
            F reader)
            : __COptionWithValue<T>(name, short_name, need, def, desc), reader(reader){
        }

    private:
        T Read(const std::string &s){
            return reader(s);
        }

        F reader;
    };

    std::map<std::string, COptionBase*> m_mapOptions;
    std::vector<COptionBase*> m_vecOrdered;
    std::string m_strFtr;

    std::string m_strProgName;
    std::vector<std::string> m_vecOthers;

    std::vector<std::string> m_vecErrors;
};

}
}
}

#endif // PUMP_CORE_CMDPARSE_HPP