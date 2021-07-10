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

#ifndef __PRIMITIVE_BASE_H__
#define __PRIMITIVE_BASE_H__
#include <string>
#include "pump_ac/jsoncpp/json.h"

namespace JsdValidator
{

    typedef enum
    {
        /////////////////////////////////普通错误码/////////////////////////////////
        JVAL_ROK = 0,
        JVAL_ERR_RESOURCE_ALLOC,  // [add] by yangzheng 2020/01/07 资源申请失败
        /////////////////////////////////json报文解析错误码 DP(Document Parse) ////////////////////////////
        JVAL_ERR_DP_JSON_INVALID = 50,
        JVAL_ERR_DP_TYPE_INVALID,       // [add] by yangzheng 2020/01/07 无效的json数据类型
        /////////////////////////////////json schema 解析错误码 SP(Schema Parse)////////////////////////////
        JVAL_ERR_SP_PATERN_INVALID = 100, // [add] by yangzheng 2020/01/07 schema中无效的通配符
        JVAL_ERR_SP_MAXLENTH_INVALID,
        JVAL_ERR_SP_MINLENTH_INVALID,
        JVAL_ERR_SP_FORMATE_INVALID,
        JVAL_ERR_SP_ENUM_INVALID,
        JVAL_ERR_SP_MINITEMS_INVALID,
        JVAL_ERR_SP_MAXITEMS_INVALID,
        JVAL_ERR_SP_UNIQUEITEMS_INVALID,
        JVAL_ERR_SP_ITEMS_INVALID,
        JVAL_ERR_SP_MINPROPERTIES_INVALID,
        JVAL_ERR_SP_MAXPROPERTIES_INVALID,
        JVAL_ERR_SP_REQUIRED_INVALID,
        JVAL_ERR_SP_PROPERTIES_INVALID,
        JVAL_ERR_SP_ADDITIONALPROPERTIES_INVALID,
        JVAL_ERR_SP_DEPENDENCIES_INVALID,
        JVAL_ERR_SP_ITEMSLIST_INVALID,
        JVAL_ERR_SP_MINIMUM_INVALID,
        JVAL_ERR_SP_EXCLUSIVEMINIMUM_INVALID,
        JVAL_ERR_SP_MAXIMUM_INVALID,
        JVAL_ERR_SP_EXCLUSIVEMAXIMUM_INVALID,
        /////////////////////////////////校验错误码//////////////////////////////
        JVAL_ERR_INVALID_MAXIMUM = 150,
        JVAL_ERR_INVALID_MINIMUM,
        JVAL_ERR_NOT_A_MULTIPLE,
        JVAL_ERR_INVALID_MIN_LENGTH,
        JVAL_ERR_INVALID_MAX_LENGTH,
        JVAL_ERR_PATTERN_MISMATCH,
        JVAL_ERR_INVALID_MAX_ITEMS,
        JVAL_ERR_INVALID_MIN_ITEMS,
        JVAL_ERR_DUPLICATE_ITEMS,
        JVAL_ERR_INVALID_MAX_PROPERTIES,
        JVAL_ERR_INVALID_MIN_PROPERTIES,
        JVAL_ERR_REQUIRED_ITEM_MISSING,
        JVAL_ERR_INVALID_ARRAY_ITEM,
        JVAL_ERR_UNKNOWN_PROPERTY,
        JVAL_ERR_INVALID_SCHEMA,
        JVAL_ERR_NOT_AN_INTEGER,
        JVAL_ERR_NOT_A_NUMBER,
        JVAL_ERR_NOT_A_STRING,
        JVAL_ERR_NOT_AN_ARRAY,
        JVAL_ERR_NOT_AN_OBJECT,
        JVAL_ERR_INVALID_PROPERTY,
        JVAL_ERR_ADDITIONAL_ITEMS,
        JVAL_ERR_FORMAT_MISMATCH,                // [add] by yangzheng 2019/11/28 format 不匹配
        JVAL_ERR_FORMAT_TYPE_UNSUPPORT,   // [add] by yangzheng 2019/11/28 format 类型不支持
        JVAL_ERR_ENUMERATION_INVALID,         // [add] by yangzheng 2019/11/29 非法 enumeration 类型
        JVAL_ERR_DEPENDENCIES_INVALID,          // [add] by yangzheng 2019/11/29 dependencies 属性非法
        JVAL_ERR_NOT_A_BOOLEAN,                      // [add] by yangzheng 2019/12/25 非boolean
        JVAL_ERR_NOT_A_NULL,                              // [add] by yangzheng 2019/12/25 非 null
        JVAL_ERR_NUMBER_EXCEED_INHERENT_LIMIT, // [add] by yangzheng 2019/12/25 number的超过固有取值范围
    } JsdValidError;

    typedef enum
    {
        JSON_TYPE_INVALID = 0,
        JSON_TYPE_ARRAY,
        JSON_TYPE_BOOLEAN,
        JSON_TYPE_ENUM,
        JSON_TYPE_INTEGER,
        JSON_TYPE_NULL,
        JSON_TYPE_NUMBER,
        JSON_TYPE_OBJECT,
        JSON_TYPE_STRING,
    } JsonPrimitiveType;

    class JVException
    {
    public:
        JVException()
            : m_err(-1)
            , m_msg()
            , m_pErrNode(NULL)
        {}

        explicit JVException(unsigned int err)
            : m_err(err)
            , m_msg()
            , m_pErrNode(NULL)
        {}

        JVException(unsigned int err, std::string const& msg)
            : m_err(err)
            , m_msg(msg)
            , m_pErrNode(NULL)
        {}

        JVException(unsigned int err, std::string const& msg, const Json::Value * pErrNode)
            : m_err(err)
            , m_msg(msg)
            , m_pErrNode(pErrNode)
        {}

        ~JVException() {}

        char const* what() const {
            return m_msg.c_str();
        }

        int error() const {
            return m_err;
        }

        const Json::Value * node() const
        {
            return m_pErrNode;
        }

        void addMsgPre(const std::string & strMsg)
        {
            m_msg = strMsg + m_msg;
        }
        void addMsgPost(const std::string & strMsg)
        {
            m_msg += strMsg;
        }

    private:
        const Json::Value * m_pErrNode;
        std::string m_msg;
        int m_err;
    };

    class JsonPrimitive
    {
    private:
        JsonPrimitiveType   m_type;

    protected:
        JsonPrimitiveType type() { return m_type; }

    public:
        JsonPrimitive(Json::Value *element) {
            m_type = getPrimitveType(element);
        }

        virtual ~JsonPrimitive() { destroy(); }

        virtual void destroy() {}

        virtual int validate(const Json::Value *value) = 0;

        static JsonPrimitiveType getPrimitveType(Json::Value *value);

        // factory method for creating type specific element validator
        static JsonPrimitive *createPrimitive(Json::Value *elment);
    };

}

#endif
