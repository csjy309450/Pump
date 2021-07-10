/******************************************************************************
 * Copyright (c) 2016, Nithin Nellikunnu (nithin.nn@gmail.com)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *****************************************************************************/

#ifndef __KEYWORD_VALIDATOR_H__
#define __KEYWORD_VALIDATOR_H__
#include "pump_macro/pump_pre.h"
#ifdef PUMP_HAS_CPP11
#include <regex>
#else
#ifdef PUMP_OS_WINDOWS
#include "gnuregex/regex_win.h"
#elif defined PUMP_OS_POSIX
#include <regex.h>
#endif // PUMP_OS_WINDOWS
#endif // !PUMP_HAS_CPP11

namespace JsdValidator
{

    class KeywordValidator
    {
    public:
        virtual ~KeywordValidator() { destroy(); };
        virtual int validate(const Json::Value *value) = 0;
        virtual void destroy() {}
    };

    class IntValid : public KeywordValidator
    {
    public:
        IntValid() {};
        ~IntValid() {};
        int validate(const Json::Value *value);
    };

    class IntMaximum : public KeywordValidator
    {
    public:
        IntMaximum(int, bool);
        ~IntMaximum() {};
        int validate(const Json::Value *value);

    private:
        int  m_maximum;
        bool m_exclusiveMaximum;
    };

    class IntMinimum : public KeywordValidator
    {
    public:
        IntMinimum(int, bool);
        ~IntMinimum() {};
        int validate(const Json::Value *value);

    private:
        bool m_exclusiveMinimum;
        int  m_minimum;
    };

    class NumberValid : public KeywordValidator
    {
    public:
        NumberValid() {};
        ~NumberValid() {};
        int validate(const Json::Value *value);
    };

    class NumberMaximum : public KeywordValidator
    {
    public:
        NumberMaximum(double, bool);
        ~NumberMaximum() {};
        int validate(const Json::Value *value);

    private:
        double   m_maximum;
        bool     m_exclusiveMaximum;
    };

    class NumberMinimum : public KeywordValidator
    {
    public:
        NumberMinimum(double, bool);
        ~NumberMinimum() {};
        int validate(const Json::Value *value);

    private:
        double   m_minimum;
        bool     m_exclusiveMinimum;
    };

    class IntMultipleOf : public KeywordValidator
    {
    public:
        IntMultipleOf(int);
        ~IntMultipleOf() {}
        int validate(const Json::Value *value);

    private:
        int m_multipleOf;
    };

    class NumberMultipleOf : public KeywordValidator
    {
    public:
        NumberMultipleOf(double);
        ~NumberMultipleOf() {}
        int validate(const Json::Value *value);

    private:
        double m_multipleOf;
    };

    class StringValid : public KeywordValidator
    {
    public:
        StringValid() {};
        ~StringValid() {};
        int validate(const Json::Value *value);
    };

    class MinLength : public KeywordValidator
    {
    public:
        MinLength(int);
        ~MinLength() {}
        int validate(const Json::Value *value);

    private:
        unsigned int m_minLength;
    };

    class MaxLength : public KeywordValidator
    {
    public:
        MaxLength(int);
        ~MaxLength() {}
        int validate(const Json::Value *value);

    private:
        unsigned int m_maxLength;
    };

    class Pattern : public KeywordValidator
    {
    public:
        Pattern(JSONCPP_STRING);
        ~Pattern();
        int validate(const Json::Value *value);
        bool isInit();

    private:
#ifdef PUMP_HAS_CPP11
        std::regex m_pattern;
#else
        regex_t m_pattern;
#endif // PUMP_HAS_CPP11
        bool m_bInit;
    };

    /*!
    * @class Format
    * @brief string 类型数据 format 属性校验
    * @author yangzheng
    * @date 2019/11/28
    * @DONE date-time 只实现最基本格式校验, 没做日期有效性校验
    */
    class  Format : public KeywordValidator
    {
    public:
        Format(JSONCPP_STRING);
        ~Format();
        int validate(const Json::Value *value);
        bool IsInit();
    private:
        std::string m_format;
        const char* m_szDatePattern;
        const char* m_szDateTimePattern;
        const char* m_szTimePattern;
        const char* m_szIpv4Pattern;
        const char* m_szIpv6Pattern;
#ifdef PUMP_HAS_CPP11
        std::regex m_dataPattern;
#else
        regex_t m_dataPattern;
#endif // PUMP_HAS_CPP11
        bool m_bInit;
        //const std::regex m_dateTimePattern;
    };

    /*!
    * @class Enum
    * @brief string 类型数据 enum 属性校验
    * @author yangzheng
    * @date 2019/11/29
    */
    class Enum : public KeywordValidator
    {
    public:
        Enum(const Json::Value & value);
        ~Enum() {};
        int validate(const Json::Value *value);
    private:
        Json::Value m_enum;
    };

    class Array : public KeywordValidator
    {
    public:
        Array() {};
        ~Array() {};
        int validate(const Json::Value *value);
    };

    class MinItems : public KeywordValidator
    {
    public:
        MinItems(unsigned int);
        ~MinItems() {}
        int validate(const Json::Value *value);

    private:
        unsigned int m_minItems;
    };

    class MaxItems : public KeywordValidator
    {
    public:
        MaxItems(unsigned int);
        ~MaxItems() {}
        int validate(const Json::Value *value);

    private:
        unsigned int m_maxItems;
    };

    class ItemsTuple : public KeywordValidator
    {
    public:
        ItemsTuple(Json::Value);
        ~ItemsTuple();
        int validate(const Json::Value *value);

    private:
        Json::Value                   m_items;
        std::vector<JsonPrimitive*>   m_primitives;
    };

    class ItemsList : public KeywordValidator
    {
    public:
        ItemsList(Json::Value items);
        ~ItemsList();
        int validate(const Json::Value *value);

    private:
        Json::Value    m_items;
        JsonPrimitive  *m_primitive;
    };

    class UniqueItems : public KeywordValidator
    {
    public:
        UniqueItems(bool);
        ~UniqueItems() {}
        int validate(const Json::Value *value);

    private:
        bool m_uniqueItems;
    };

    class AdditionalItems : public KeywordValidator
    {
    public:
        AdditionalItems(unsigned int);
        ~AdditionalItems() {}
        int validate(const Json::Value *value);

    private:
        unsigned int m_itemsSize;
    };

    class ObjectValid : public KeywordValidator
    {
    public:
        ObjectValid() {};
        ~ObjectValid() {};
        int validate(const Json::Value *value);
    };

    class MaxProperties : public KeywordValidator
    {
    public:
        MaxProperties(unsigned int);
        ~MaxProperties() {}
        int validate(const Json::Value *value);

    private:
        unsigned int m_maxProperties;
    };

    class MinProperties : public KeywordValidator
    {
    public:
        MinProperties(unsigned int);
        ~MinProperties() {}
        int validate(const Json::Value *value);

    private:
        unsigned int m_minProperties;
    };

    class Required : public KeywordValidator
    {
    public:
        Required(Json::Value);
        ~Required() {}
        int validate(const Json::Value *value);
    private:
        std::vector<std::string> m_required;
    };

    class Properties : public KeywordValidator
    {
    public:
        Properties(Json::Value, bool);
        ~Properties();
        int validate(const Json::Value *value);
        virtual void destroy();

    private:
        Json::Value                            m_properties;
        bool                                   m_additionalProperties;
        std::map<std::string, JsonPrimitive*>  m_primitives;
    };

    /*!
    * @class Dependencies
    * @brief
    * @author yangzheng
    * @date 2019/11/29
    */
    class Dependencies : public KeywordValidator
    {
    public:
        Dependencies(const Json::Value & val);
        ~Dependencies(){}
        int validate(const Json::Value *value);

    private:
        Json::Value m_dep;
    };

    class AdditionalProperties
    {
    public:
        AdditionalProperties();
        ~AdditionalProperties() {}
        int validate(const Json::Value *value);

    private:
        bool m_additionalProperties;
    };

}

#endif
