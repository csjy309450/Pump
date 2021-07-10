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

#include <string>
#include <fstream>
#include <streambuf>
#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <stdexcept>
#include <algorithm>
//#include <regex>
#include "pump_ac/jsoncpp/json.h"
#include "pump_ac/jsoncpp/primitive_base.h"
#include "pump_ac/jsoncpp/keyword_validator.h"
#include "pump_ac/jsoncpp/primitive.h"
#include "pump_ac/jsoncpp/validator.h"
namespace JsdValidator
{
    JsonValidator::JsonValidator()
    {
        m_primitive = NULL;
    }

    JsonValidator::JsonValidator(std::string &schema)
    {
        m_primitive = NULL;
        parseSchema(schema);
    }

    JsonValidator::JsonValidator(Json::Value *schema)
    {
        m_primitive = NULL;
        m_primitive = JsonPrimitive::createPrimitive(schema);
    }

    JsonValidator::~JsonValidator()
    {
        delete m_primitive;
    }

    void JsonValidator::readSchema(const char *schema_file)
    {
        std::ifstream t(schema_file);

        // reserver memory for entire schema instead of string class automatic
        // reallocation
        std::string str;
        t.seekg(0, std::ios::end);
        str.reserve((std::string::size_type)t.tellg());
        t.seekg(0, std::ios::beg);

        str.assign((std::istreambuf_iterator<char>(t)),
            std::istreambuf_iterator<char>());

        parseSchema(str);
    }

    void JsonValidator::parseSchema(std::string &str)
    {
        Json::Reader reader;
        Json::Value * schema = new (std::nothrow) Json::Value();
        if (!schema)
        {
            throw JVException(JVAL_ERR_RESOURCE_ALLOC);
        }
        bool parsingSuccessful = reader.parse(str, *schema);
        if (!parsingSuccessful) {
            delete schema;
            throw JVException(JVAL_ERR_DP_JSON_INVALID, reader.getFormattedErrorMessages());
        }

        try
        {
            m_primitive = JsonPrimitive::createPrimitive(schema);
        }
        catch (JsdValidator::JVException &e)
        {
            delete schema;
            std::string strMsg("Invalid Schema,");
            strMsg += e.what();
            throw JVException(e.error(), strMsg);
        }
        m_schema = schema;
    }

    int JsonValidator::validate(const Json::Value *value)
    {
        int ret = JVAL_ROK;

        try {
            ret = m_primitive->validate(value);
        }
        catch (JsdValidator::JVException &e) {
            e.addMsgPre("Validate failed,");
            throw e;
        }

        return ret;
    }

    Json::Value * JsonValidator::getSchema()
    {
        return m_schema;
    }

    /**
     * @brief Creates the primitive type based on the schema
     *
     * @param schema contains schema of the form
     *    {
     *       "type" : "integer"
     *       ...
     *       ...
     *    }
     *
     * @return
     */
    JsonPrimitive *JsonPrimitive::createPrimitive(Json::Value *schema)
    {
        JsonPrimitiveType type = JsonPrimitive::getPrimitveType(schema);

        switch (type) {
        case JSON_TYPE_INTEGER:
        {
            JsonPrimitive * pJPrim = NULL;
            try
            {
                pJPrim = new JsonInteger(schema);
            }
            catch (JsdValidator::JVException &e)
            {
                if (pJPrim)
                {
                    delete pJPrim;
                    pJPrim = NULL;
                }
                throw e;
            }
            return pJPrim;
        }
        case JSON_TYPE_NUMBER:
        {
            JsonPrimitive * pJPrim = NULL;
            try
            {
                pJPrim = new JsonNumber(schema);
            }
            catch (JsdValidator::JVException &e)
            {
                if (pJPrim)
                {
                    delete pJPrim;
                    pJPrim = NULL;
                }
                throw e;
            }
            return pJPrim;
        }
        case JSON_TYPE_STRING:
        {
            JsonPrimitive * pJPrim = NULL;
            try
            {
                pJPrim = new JsonString(schema);
            }
            catch (JsdValidator::JVException &e)
            {
                if (pJPrim)
                {
                    delete pJPrim;
                    pJPrim = NULL;
                }
                throw e;
            }
            return pJPrim;
        }
        case JSON_TYPE_OBJECT:
        {
            JsonPrimitive * pJPrim = NULL;
            try
            {
                pJPrim = new JsonObject(schema);
            }
            catch (JsdValidator::JVException &e)
            {
                if (pJPrim)
                {
                    delete pJPrim;
                    pJPrim = NULL;
                }
                throw e;
            }
            return pJPrim;
        }
        case JSON_TYPE_ARRAY:
        {
            JsonPrimitive * pJPrim = NULL;
            try
            {
                pJPrim = new JsonArray(schema);
            }
            catch (JsdValidator::JVException &e)
            {
                if (pJPrim)
                {
                    delete pJPrim;
                    pJPrim = NULL;
                }
                throw e;
            }
            return pJPrim;
        }
        case JSON_TYPE_BOOLEAN:
        {
            JsonPrimitive * pJPrim = NULL;
            try
            {
                pJPrim = new JsonBoolean(schema);
            }
            catch (JsdValidator::JVException &e)
            {
                if (pJPrim)
                {
                    delete pJPrim;
                    pJPrim = NULL;
                }
                throw e;
            }
            return pJPrim;
        }
        case JSON_TYPE_NULL:
        {
            JsonPrimitive * pJPrim = NULL;
            try
            {
                pJPrim = new JsonNull(schema);
            }
            catch (JsdValidator::JVException &e)
            {
                if (pJPrim)
                {
                    delete pJPrim;
                    pJPrim = NULL;
                }
                throw e;
            }
            return pJPrim;
        }
        default:
            throw JVException(JVAL_ERR_DP_TYPE_INVALID, "unknown JsonPrimitiveType");
        }
    }

    /**
     * @brief Converts type-specific json schema keywords into enum values
     *
     * @param schema contains schema of the form
     *    {
     *       "type" : "integer"
     *       ...
     *       ...
     *    }
     *
     * @return
     */
    JsonPrimitiveType JsonPrimitive::getPrimitveType(Json::Value *schema)
    {
        if (schema->isMember("type")) {
            Json::Value typeValue = schema->get("type", typeValue);
            if (typeValue.asString() == "integer") {
                return JSON_TYPE_INTEGER;
            }
            else if (typeValue.asString() == "number") {
                return JSON_TYPE_NUMBER;
            }
            else if (typeValue.asString() == "string") {
                return JSON_TYPE_STRING;
            }
            else if (typeValue.asString() == "object") {
                return JSON_TYPE_OBJECT;
            }
            else if (typeValue.asString() == "array") {
                return JSON_TYPE_ARRAY;
            }
            else if (typeValue.asString() == "boolean") {
                return JSON_TYPE_BOOLEAN;
            }
            else if (typeValue.asString() == "null") {
                return JSON_TYPE_NULL;
            }
            else {
                std::string err = "Unknown type keyword \"" + \
                    typeValue.asString() + "\"";
                throw JVException(JVAL_ERR_DP_TYPE_INVALID, err);
            }
        }
        else {
            throw JVException(JVAL_ERR_DP_TYPE_INVALID, "\"type\" keyword is undefined");
        }
    }

}


