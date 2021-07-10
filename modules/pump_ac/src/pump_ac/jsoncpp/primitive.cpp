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
#include <cassert>
//#include <regex>
#include "pump_ac/jsoncpp/json.h"
#include "pump_ac/jsoncpp/primitive_base.h"
#include "pump_ac/jsoncpp/keyword_validator.h"
#include "pump_ac/jsoncpp/primitive.h"

namespace JsdValidator
{

    JsonBoolean::JsonBoolean(Json::Value *element) : JsonPrimitive(element)
    {
    }

    int JsonBoolean::validate(const Json::Value *value)
    {
        (void)value;
        if (!value->isBool())
        {
            throw JVException(JVAL_ERR_NOT_A_BOOLEAN, "JsonBoolean error");
        }
        return JVAL_ROK;
    }

    JsonNull::JsonNull(Json::Value *element) : JsonPrimitive(element)
    {
    }

    int JsonNull::validate(const Json::Value *value)
    {
        (void)value;
        if (!value->isNull())
        {
            throw JVException(JVAL_ERR_NOT_A_NULL, "JsonNull error");
        }
        return JVAL_ROK;
    }

    JsonInteger::JsonInteger(Json::Value *schema) : JsonPrimitive(schema)
    {
        // validator for integer type
        m_validators.push_back(new IntValid);

        // validator for minimum check
        if (schema->isMember("minimum")) {
            Json::Value min = schema->get("minimum", min);
            if (!min.isInt())
            {
                destroy();
                throw JVException(JVAL_ERR_SP_MINIMUM_INVALID, "Invalid JsonInteger,Invalid minimum");
            }
            bool exclusiveMin = false;
            if (schema->isMember("exclusiveMinimum")) {
                Json::Value eMin = schema->get("exclusiveMinimum", eMin);
                if (!eMin.isBool())
                {
                    destroy();
                    throw JVException(JVAL_ERR_SP_EXCLUSIVEMINIMUM_INVALID, "Invalid JsonInteger,Invalid exclusiveMinimum");
                }
                exclusiveMin = eMin.asBool();
            }

            IntMinimum *minimum = new IntMinimum(min.asInt(), exclusiveMin);
            m_validators.push_back(minimum);
        }

        // validator for maximum check
        if (schema->isMember("maximum")) {
            Json::Value max = schema->get("maximum", max);
            if (!max.isInt())
            {
                destroy();
                throw JVException(JVAL_ERR_SP_MAXIMUM_INVALID, "Invalid JsonInteger,Invalid maximum");
            }
            bool exclusiveMax = false;
            if (schema->isMember("exclusiveMaximum")) {
                Json::Value eMax = schema->get("exclusiveMaximum", eMax);
                if (!eMax.isBool())
                {
                    destroy();
                    throw JVException(JVAL_ERR_SP_EXCLUSIVEMAXIMUM_INVALID, "Invalid JsonInteger exclusiveMaximum");
                }
                exclusiveMax = eMax.asBool();
            }

            NumberMaximum *maximum = new NumberMaximum(max.asInt(), exclusiveMax);
            m_validators.push_back(maximum);
        }

        // validator for multipleOf
        if (schema->isMember("multipleOf")) {
            Json::Value multipleOf = schema->get("multipleOf", multipleOf);
            m_validators.push_back(new IntMultipleOf(multipleOf.asInt()));
        }
    }

    JsonInteger::~JsonInteger()
    {
    }

    void JsonInteger::destroy()
    {
        for (std::list<KeywordValidator*>::iterator itr = m_validators.begin();
            itr != m_validators.end();
            itr++)
        {
            delete *itr;
        }
    }

    int JsonInteger::validate(const Json::Value *value)
    {
        for (std::list<KeywordValidator*>::iterator b = m_validators.begin();
            b != m_validators.end();
            b++) {
            try
            {
                (*b)->validate(value);
            }
            catch (JVException& e)
            {
                e.addMsgPre("JsonInteger error,");
                throw e;
            }
        }

        return JVAL_ROK;
    }

    JsonNumber::JsonNumber(Json::Value *schema) : JsonPrimitive(schema)
    {
        // validator for integer type
        m_validators.push_back(new NumberValid());

        // validator for minimum check
        if (schema->isMember("minimum")) {
            Json::Value min = schema->get("minimum", min);
            if (!min.isInt())
            {
                destroy();
                throw JVException(JVAL_ERR_SP_MINIMUM_INVALID, "Invalid JsonNumber,Invalid minimum");
            }

            bool exclusiveMin = false;
            if (schema->isMember("exclusiveMinimum")) {
                Json::Value eMin = schema->get("exclusiveMinimum", eMin);
                if (!eMin.isBool())
                {
                    destroy();
                    throw JVException(JVAL_ERR_SP_EXCLUSIVEMINIMUM_INVALID, "Invalid JsonNumber,Invalid exclusiveMinimum");
                }
                exclusiveMin = eMin.asBool();
            }

            NumberMinimum *minimum = new NumberMinimum(min.asInt(), exclusiveMin);
            m_validators.push_back(minimum);
        }

        // validator for maximum check
        if (schema->isMember("maximum")) {
            Json::Value max = schema->get("maximum", max);
            if (!max.isInt())
            {
                destroy();
                throw JVException(JVAL_ERR_SP_MAXIMUM_INVALID, "Invalid JsonNumber,Invalid maximum");
            }

            bool exclusiveMax = false;
            if (schema->isMember("exclusiveMaximum")) {
                Json::Value eMax = schema->get("exclusiveMaximum", eMax);
                if (!eMax.isBool())
                {
                    destroy();
                    throw JVException(JVAL_ERR_SP_EXCLUSIVEMAXIMUM_INVALID, "Invalid JsonNumber,Invalid exclusiveMaximum");
                }
                exclusiveMax = eMax.asBool();
            }

            IntMaximum *maximum = new IntMaximum(max.asInt(), exclusiveMax);
            m_validators.push_back(maximum);
        }

        // validator for multipleOf
        if (schema->isMember("multipleOf")) {
            Json::Value multipleOf = schema->get("multipleOf", multipleOf);
            m_validators.push_back(new NumberMultipleOf(multipleOf.asDouble()));
        }

        // [add] by yangzheng 2019/11/29 string enum
        if (schema->isMember("enum")) {
            Json::Value value_enum = schema->get("enum", value_enum);
            if (!value_enum.isArray())
            {
                destroy();
                throw JVException(JVAL_ERR_SP_ENUM_INVALID, "Invalid JsonNumber,Invalid enum");
            }
            m_validators.push_back(new Enum(value_enum));
        }
    }

    JsonNumber::~JsonNumber()
    {
    }

    void JsonNumber::destroy()
    {
        for (std::list<KeywordValidator*>::iterator itr = m_validators.begin();
            itr != m_validators.end();
            itr++)
        {
            delete *itr;
        }
    }

    int JsonNumber::validate(const Json::Value *value)
    {
        for (std::list<KeywordValidator*>::iterator b = m_validators.begin();
            b != m_validators.end();
            b++) 
        {
            try
            {
                (*b)->validate(value);
            }
            catch (JVException& e)
            {
                e.addMsgPre("JsonNumber error,");
                throw e;
            }
        }

        return JVAL_ROK;
    }

    JsonString::JsonString(Json::Value *schema) : JsonPrimitive(schema)
    {
        m_validators.push_back(new StringValid);

        // checking length constraints
        if (schema->isMember("minLength")) {
            Json::Value min = schema->get("minLength", min);
            if (!min.isUInt())
            {
                destroy();
                throw JVException(JVAL_ERR_SP_MINLENTH_INVALID, "Invalid JsonString,Invalid minLength");
            }
            size_t length = min.asUInt();
            m_validators.push_back(new MinLength(length));
        }

        if (schema->isMember("maxLength")) {
            Json::Value max = schema->get("maxLength", max);
            if (!max.isUInt())
            {
                destroy();
                throw JVException(JVAL_ERR_SP_MAXLENTH_INVALID, "Invalid JsonString,Invalid maxLength");
            }
            size_t length = max.asUInt();
            m_validators.push_back(new MaxLength(length));
        }

        // string pattern
        if (schema->isMember("pattern")) {
            Json::Value pattern = schema->get("pattern", pattern);
            if (!pattern.isString())
            {
                destroy();
                throw JVException(JVAL_ERR_SP_PATERN_INVALID, "Invalid JsonString,Invalid pattern");
            }
            Pattern * pPattern = NULL;
            try
            {
                pPattern = new Pattern(pattern.asString());
            }
#ifdef PUMP_HAS_CPP11
            catch (std::exception &e)
            {
                delete pPattern;
                destroy();
                throw JVException(JVAL_ERR_SP_PATERN_INVALID, std::string("Invalid JsonString,Invalid pattern(") + e.what() + ")");
            }
#else
            catch(JsdValidator::JVException &e)
            {
                if (!pPattern->isInit())
                {
                    delete pPattern;
                    destroy();
                    e.addMsgPre("Invalid JsonString,");
                    throw e;
                }
            }
#endif
            m_validators.push_back(pPattern);
        }

        // [add] by yangzheng 2019/11/28 string format
        if (schema->isMember("format")) {
            Json::Value format = schema->get("format", format);
            if (!format.isString())
            {
                destroy();
                throw JVException(JVAL_ERR_SP_FORMATE_INVALID, "Invalid JsonString,Invalid format");
            }
            Format * pFormat = new Format(format.asString());
            if (!pFormat->IsInit())
            {
                // 进到此处说明format的通配符有bug
                assert(1 == 0);
            }
            m_validators.push_back(pFormat);
        }

        // [add] by yangzheng 2019/11/29 string enum
        if (schema->isMember("enum")) {
            Json::Value value_enum = schema->get("enum", value_enum);
            if (!value_enum.isArray())
            {
                destroy();
                throw JVException(JVAL_ERR_SP_ENUM_INVALID, "Invalid JsonString,Invalid enum");
            }
            m_validators.push_back(new Enum(value_enum));
        }
    }

    JsonString::~JsonString()
    {
    }

    void JsonString::destroy()
    {
        for (std::list<KeywordValidator*>::iterator itr = m_validators.begin();
            itr != m_validators.end();
            itr++)
        {
            delete *itr;
        }
    }

    int JsonString::validate(const Json::Value *value)
    {
        for (std::list<KeywordValidator*>::iterator b = m_validators.begin();
            b != m_validators.end();
            b++) 
        {
            try
            {
                (*b)->validate(value);
            }
            catch (JVException& e)
            {
                e.addMsgPre("JsonString error,");
                throw e;
            }
        }

        return JVAL_ROK;
    }

    JsonArray::JsonArray(Json::Value *schema) : JsonPrimitive(schema)
    {
        m_validators.push_back(new Array);

        // checking minimum number of items constraint
        if (schema->isMember("minItems")) {
            Json::Value min = schema->get("minItems", min);
            if (!min.isUInt())
            {
                destroy();
                throw JVException(JVAL_ERR_SP_MINITEMS_INVALID, "Invalid JsonArray,Invalid minItems");
            }
            m_validators.push_back(new MinItems(min.asUInt()));
        }

        // checking maximum number of items constraint
        if (schema->isMember("maxItems")) {
            Json::Value max = schema->get("maxItems", max);
            if (!max.isUInt())
            {
                destroy();
                throw JVException(JVAL_ERR_SP_MAXITEMS_INVALID, "Invalid JsonArray,Invalid maxItems");
            }
            m_validators.push_back(new MaxItems(max.asUInt()));
        }

        // checking maximum number of items constraint
        if (schema->isMember("uniqueItems")) {
            Json::Value unique = schema->get("uniqueItems", unique);
            if (!unique.isBool())
            {
                destroy();
                throw JVException(JVAL_ERR_SP_UNIQUEITEMS_INVALID, "Invalid JsonArray,Invalid uniqueItems");
            }
            m_validators.push_back(new UniqueItems(unique.asBool()));
        }

        // items
        if (schema->isMember("items")) {
            Json::Value items = schema->get("items", items);
            if (items.isArray())
            {
                // if items in the json array is of tuple type then additionalitems
                // keyword will be used for the validation
                if (schema->isMember("additionalItems")) {
                    Json::Value ai = schema->get("additionalItems", ai);

                    // if additinalItems can be boolean or object. If it is an object
                    // the validation of tuple always succeeds
                    // the Only time additionalitems is useful when the array is tuple
                    // and additionalitems == false
                    if (ai.type() == Json::booleanValue && \
                        ai.asBool() == false) {
                        m_validators.push_back(new AdditionalItems(items.size()));
                    }
                }

                m_validators.push_back(new ItemsTuple(items));
            }
            else if (items.isObject())
            {
                // ignore the additionalitems keyword even if present in the schema
                m_validators.push_back(new ItemsList(items));
            }
            else
            {
                destroy();
                throw JVException(JVAL_ERR_SP_ITEMS_INVALID, "Invalid JsonArray,Invalid items");
            }
        }
    }

    JsonArray::~JsonArray()
    {

    }

    void JsonArray::destroy()
    {
        for (std::list<KeywordValidator*>::iterator itr = m_validators.begin();
            itr != m_validators.end();
            itr++)
        {
            delete *itr;
        }
    }

    int JsonArray::validate(const Json::Value *value)
    {
        for (std::list<KeywordValidator*>::iterator b = m_validators.begin();
            b != m_validators.end();
            b++)
        {
            try
            {
                (*b)->validate(value);
            }
            catch (JVException& e)
            {
                e.addMsgPre("JsonArray error,");
                throw e;
            }
        }

        return JVAL_ROK;
    }

    JsonObject::JsonObject(Json::Value *schema) : JsonPrimitive(schema)
    {
        m_validators.push_back(new ObjectValid);

        // checking minimum number of properties constraint
        if (schema->isMember("minProperties")) {
            Json::Value min = schema->get("minProperties", min);
            if (!min.isUInt())
            {
                destroy();
                throw JVException(JVAL_ERR_SP_MINPROPERTIES_INVALID, "Invalid JsonObject,Invalid minProperties");
            }
            m_validators.push_back(new MinProperties(min.asUInt()));
        }

        // checking maximum number of properties constraint
        if (schema->isMember("maxProperties")) {
            Json::Value max = schema->get("maxProperties", max);
            if (!max.isUInt())
            {
                destroy();
                throw JVException(JVAL_ERR_SP_MAXPROPERTIES_INVALID, "Invalid JsonObject,Invalid maxProperties");
            }
            m_validators.push_back(new MaxProperties(max.asUInt()));
        }

        // required properties
        if (schema->isMember("required")) {
            Json::Value req = schema->get("required", req);
            if (!req.isArray())
            {
                destroy();
                throw JVException(JVAL_ERR_SP_REQUIRED_INVALID, "Invalid JsonObject,Invalid required");
            }
            m_validators.push_back(new Required(req));
        }

        // properties
        if (schema->isMember("properties")) {
            Json::Value properties = schema->get("properties", properties);
            if (!properties.isObject())
            {
                destroy();
                throw JVException(JVAL_ERR_SP_PROPERTIES_INVALID, "Invalid JsonObject,Invalid properties");
            }
            bool additionalProperties = false;
            if (schema->isMember("additionalProperties")) {
                Json::Value v = schema->get("additionalProperties", v);
                if (!v.isBool())
                {
                    destroy();
                    throw JVException(JVAL_ERR_SP_ADDITIONALPROPERTIES_INVALID, "Invalid JsonObject,Invalid additionalProperties");
                }
                additionalProperties = v.asBool();
            }

            Properties * pProperties = NULL;
            try
            {
                pProperties = new Properties(properties, additionalProperties);
            }
            catch (JsdValidator::JVException &e)
            {
                delete pProperties;
                destroy();
                e.addMsgPre("Invalid JsonObject,");
                throw e;
            }
            m_validators.push_back(pProperties);
        }

        // [add] by yangzheng 2019/11/29
        if (schema->isMember("dependencies")) {
            Json::Value dep = schema->get("dependencies", dep);
            if (!dep.isObject())
            {
                destroy();
                throw JVException(JVAL_ERR_SP_DEPENDENCIES_INVALID, "Invalid JsonObject,Invalid dependencies");
            }
            m_validators.push_back(new Dependencies(dep));
        }
    }

    JsonObject::~JsonObject()
    {

    }

    void JsonObject::destroy()
    {
        for (std::list<KeywordValidator*>::iterator itr = m_validators.begin();
            itr != m_validators.end();
            itr++)
        {
            delete *itr;
        }
    }

    int JsonObject::validate(const Json::Value *value)
    {
        for (std::list<KeywordValidator*>::iterator b = m_validators.begin();
            b != m_validators.end();
            b++) 
        {
            try
            {
                (*b)->validate(value);
            }
            catch (JVException& e)
            {
                e.addMsgPre("JsonObject error,");
                throw e;
            }
        }

        return JVAL_ROK;
    }

}

