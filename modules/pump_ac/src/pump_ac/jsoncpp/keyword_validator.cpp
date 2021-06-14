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

#include <iostream>
#include <string>
#include <cmath>
#include <limits>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <cassert>
//#include <regex>
#include "pump_ac/jsoncpp/json.h"
#include "pump_ac/jsoncpp/primitive_base.h"
#include "pump_ac/jsoncpp/keyword_validator.h"

namespace JsdValidator
{

    inline bool almostEqual(double a, double b, double errorFactor = 2.0)
    {
        return (a == b) || ::abs(a - b) < std::abs(min(a, b)) * \
            std::numeric_limits<double>::epsilon() * errorFactor;
    }

    int IntValid::validate(const Json::Value *value)
    {
        if (!value->isInt()) {
            throw JVException(JVAL_ERR_NOT_AN_INTEGER, "IntValid error", value);
        }

        return JVAL_ROK;
    }

    IntMaximum::IntMaximum(int maximum, bool exclusiveMaximum = false)
    {
        m_maximum = maximum;
        m_exclusiveMaximum = exclusiveMaximum;
    }

    int IntMaximum::validate(const Json::Value *value)
    {
        Json::Int val;
        try
        {
            val = value->asInt();
        }
        catch (Json::LogicError e)
        {
            throw JVException(JVAL_ERR_NUMBER_EXCEED_INHERENT_LIMIT, "IntMaximum error");
        }
        if (val > m_maximum) {
            throw JVException(JVAL_ERR_INVALID_MAXIMUM, "IntMaximum error");
        }

        if (val == m_maximum && m_exclusiveMaximum == true) {
            throw JVException(JVAL_ERR_INVALID_MAXIMUM, "IntMaximum error");
        }

        return JVAL_ROK;
    }

    IntMinimum::IntMinimum(int minimum, bool exclusiveMinimum = false)
    {
        m_minimum = minimum;
        m_exclusiveMinimum = exclusiveMinimum;
    }

    int IntMinimum::validate(const Json::Value *value)
    {
        Json::Int val;
        try
        {
            val = value->asInt();
        }
        catch (Json::LogicError e)
        {
            throw JVException(JVAL_ERR_NUMBER_EXCEED_INHERENT_LIMIT, "IntMinimum  error");
        }
        if (val < m_minimum) {
            throw JVException(JVAL_ERR_INVALID_MINIMUM, "IntMinimum  error");
        }

        if (val == m_minimum && m_exclusiveMinimum == true) {
            throw JVException(JVAL_ERR_INVALID_MINIMUM, "IntMinimum error");
        }

        return JVAL_ROK;
    }

    int NumberValid::validate(const Json::Value *value)
    {
        if (!value->isNumeric()) {
            throw JVException(JVAL_ERR_NOT_A_NUMBER, "NumberValid error");
        }

        return JVAL_ROK;
    }

    NumberMaximum::NumberMaximum(double maximum, bool exclusiveMaximum = false)
    {
        m_maximum = maximum;
        m_exclusiveMaximum = exclusiveMaximum;
    }

    int NumberMaximum::validate(const Json::Value *value)
    {
        double val;
        try
        {
            val = value->asDouble();
        }
        catch (Json::LogicError e)
        {
            throw JVException(JVAL_ERR_NUMBER_EXCEED_INHERENT_LIMIT, "NumberMaximum error");
        }
        if (val > m_maximum) {
            throw JVException(JVAL_ERR_INVALID_MAXIMUM, "NumberMaximum error");
        }

        if (val == m_maximum && m_exclusiveMaximum == true) {
            throw JVException(JVAL_ERR_INVALID_MAXIMUM, "NumberMaximum error");
        }

        return JVAL_ROK;
    }

    NumberMinimum::NumberMinimum(double minimum, bool exclusiveMinimum = false)
    {
        m_minimum = minimum;
        m_exclusiveMinimum = exclusiveMinimum;
    }

    int NumberMinimum::validate(const Json::Value *value)
    {
        double val;
        try
        {
            val = value->asDouble();
        }
        catch (Json::LogicError e)
        {
            throw JVException(JVAL_ERR_NUMBER_EXCEED_INHERENT_LIMIT, "NumberMinimum error");
        }
        if (val < m_minimum) {
            throw JVException(JVAL_ERR_INVALID_MINIMUM, "NumberMinimum error");
        }

        if (val == m_minimum && m_exclusiveMinimum == true) {
            throw JVException(JVAL_ERR_INVALID_MINIMUM, "NumberMinimum error");
        }

        return JVAL_ROK;
    }

    IntMultipleOf::IntMultipleOf(int multipleOf = 1)
    {
        m_multipleOf = multipleOf;
    }

    int IntMultipleOf::validate(const Json::Value *value)
    {
        int val;
        try
        {
            val = value->asInt();
        }
        catch (Json::LogicError e)
        {
            throw JVException(JVAL_ERR_NUMBER_EXCEED_INHERENT_LIMIT, "IntMultipleOf error");
        }
        if ((val % m_multipleOf) != 0) {
            throw JVException(JVAL_ERR_NOT_A_MULTIPLE, "IntMultipleOf error");
        }

        return JVAL_ROK;
    }

    NumberMultipleOf::NumberMultipleOf(double multipleOf)
    {
        m_multipleOf = multipleOf;
    }

    int NumberMultipleOf::validate(const Json::Value *value)
    {
        double val;
        try
        {
            val = value->asDouble();
        }
        catch (Json::LogicError e)
        {
            throw JVException(JVAL_ERR_NUMBER_EXCEED_INHERENT_LIMIT, "NumberMultipleOf error");
        }
        double remainder = fmod(val, m_multipleOf);

        if (almostEqual(remainder, 0.0) || almostEqual(remainder, m_multipleOf)) {
            return JVAL_ROK;
        }

        throw JVException(JVAL_ERR_NOT_A_MULTIPLE, "NumberMultipleOf error");
    }

    int StringValid::validate(const Json::Value *value)
    {
        if (!value->isString()) {
            throw JVException(JVAL_ERR_NOT_A_STRING, "StringValid error");
        }

        return JVAL_ROK;
    }

    MinLength::MinLength(int minLength = 0)
    {
        m_minLength = minLength;
    }

    int MinLength::validate(const Json::Value *value)
    {
        if (value->asString().size() < m_minLength) {
            throw JVException(JVAL_ERR_INVALID_MIN_LENGTH, "MinLength error");
        }

        return JVAL_ROK;
    }

    MaxLength::MaxLength(int maxLength = 0)
    {
        m_maxLength = maxLength;
    }

    int MaxLength::validate(const Json::Value *value)
    {
        if (value->asString().size() > m_maxLength) {
            throw JVException(JVAL_ERR_INVALID_MAX_LENGTH, "MaxLength error");
        }

        return JVAL_ROK;
    }

    Pattern::Pattern(JSONCPP_STRING pattern = ".*")
        : m_bInit(false)
#ifdef PUMP_HAS_CPP11
        , m_pattern(pattern) // @note 构造失败会抛异常 
#endif // PUMP_HAS_CPP11
    {
#ifdef PUMP_HAS_CPP11
#else
        pattern = "^" + pattern;
        pattern += "$";
        int ret = regcomp(&m_pattern, pattern.c_str(), REG_EXTENDED);
        if (ret != 0)
        {
            throw JVException(JVAL_ERR_SP_PATERN_INVALID, "Invalide Pattern");
        }
#endif // PUMP_HAS_CPP11
        m_bInit = true;
    }

    Pattern::~Pattern()
    {
#ifdef PUMP_HAS_CPP11
#else
        regfree(&m_pattern);
#endif // PUMP_HAS_CPP11
    }

    bool Pattern::isInit()
    {
        return m_bInit;
    }

    int Pattern::validate(const Json::Value *value)
    {
        if (!m_bInit)
        {
            // 理论上不能进到此处
            assert(1 == 0);
        }
#ifdef PUMP_HAS_CPP11
        if (!std::regex_match(value->asString(), m_pattern)) {
            throw JVException(JVAL_ERR_PATTERN_MISMATCH, "Pattern error");
        }
#else
        regmatch_t matchs[20];
        if (!regexec(&m_pattern, value->asString().c_str(), 20, matchs, 0))
        {
            throw JVException(JVAL_ERR_PATTERN_MISMATCH, "Pattern error");
        }
#endif // PUMP_HAS_CPP11

        return JVAL_ROK;
    }

    Format::Format(JSONCPP_STRING format)
        : m_format(format)
        , m_szDatePattern("^([0-9][0-9][0-9][0-9])-(10|11|12|0[0-9])-([0-3][0-9])$")
        , m_szDateTimePattern("^([0-9][0-9][0-9][0-9])-(10|11|12|0[0-9])-([0-3][0-9])T(20|21|22|23|[0-1][0-9]):[0-5][0-9]:[0-5][0-9](\\+([0-9][0-9]):([0-9][0-9]))?$")
        , m_szTimePattern("^((20|21|22|23|[0-1][0-9]):[0-5][0-9]:[0-5][0-9])(\\+([0-9][0-9]):([0-9][0-9]))?$")
        , m_szIpv4Pattern("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$")
        , m_szIpv6Pattern("^([0-9a-fA-F]{1,4}:){6}((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$|^::([0-9a-fA-F]{1,4}:){0,4}((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$|^([0-9a-fA-F]{1,4}:):([0-9a-fA-F]{1,4}:){0,3}((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$|^([0-9a-fA-F]{1,4}:){2}:([0-9a-fA-F]{1,4}:){0,2}((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$|^([0-9a-fA-F]{1,4}:){3}:([0-9a-fA-F]{1,4}:){0,1}((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$|^([0-9a-fA-F]{1,4}:){4}:((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$|^([0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4}$|^:((:[0-9a-fA-F]{1,4}){1,6}|:)$|^[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,5}|:)$|^([0-9a-fA-F]{1,4}:){2}((:[0-9a-fA-F]{1,4}){1,4}|:)$|^([0-9a-fA-F]{1,4}:){3}((:[0-9a-fA-F]{1,4}){1,3}|:)$|^([0-9a-fA-F]{1,4}:){4}((:[0-9a-fA-F]{1,4}){1,2}|:)$|^([0-9a-fA-F]{1,4}:){5}:([0-9a-fA-F]{1,4})?$|^([0-9a-fA-F]{1,4}:){6}:$")
        , m_bInit(false)
    {
        if (m_format == "date-time")
        {
#ifdef PUMP_HAS_CPP11
            m_dataPattern = std::regex(m_szDateTimePattern);
#else
            if (regcomp(&m_dataPattern, m_szDateTimePattern, REG_EXTENDED) != 0)
            {
                m_bInit = false;
                return;
            }
#endif
            m_bInit = true;
        }
        else if (m_format == "time")
        {
#ifdef PUMP_HAS_CPP11
            m_dataPattern = std::regex(m_szTimePattern);
#else
            if (regcomp(&m_dataPattern, m_szTimePattern, REG_EXTENDED) != 0)
            {
                m_bInit = false;
                return;
            }
#endif
            m_bInit = true;
        }
        else if (m_format == "ipv4")
        {
#ifdef PUMP_HAS_CPP11
            m_dataPattern = std::regex(m_szIpv4Pattern);
#else
            if (regcomp(&m_dataPattern, m_szIpv4Pattern, REG_EXTENDED) != 0)
            {
                m_bInit = false;
                return;
            }
#endif
            m_bInit = true;
        }
        else if (m_format == "ipv6")
        {
#ifdef PUMP_HAS_CPP11
            m_dataPattern = std::regex(m_szIpv6Pattern);
#else
            if (regcomp(&m_dataPattern, m_szIpv6Pattern, REG_EXTENDED) != 0)
            {
                m_bInit = false;
                return;
            }
#endif
            m_bInit = true;
        }
        else if (m_format == "date")
        {
#ifdef PUMP_HAS_CPP11
            m_dataPattern = std::regex(m_szDatePattern);
#else
            if (regcomp(&m_dataPattern, m_szDatePattern, REG_EXTENDED) != 0)
            {
                m_bInit = false;
                return;
            }
#endif
            m_bInit = true;
        }
        else // 不支持的format
        {
            m_bInit = false;
            throw JVException(JVAL_ERR_SP_FORMATE_INVALID, "Invalid Format");
        }
    }

    Format::~Format()
    {
#ifdef PUMP_HAS_CPP11
        //
#else
        regfree(&m_dataPattern);
#endif // PUMP_HAS_CPP11
    }

    bool Format::IsInit()
    {
        return m_bInit;
    }

    int Format::validate(const Json::Value *value)
    {
        if (!m_bInit)
        {
            // 理论上进不到这里
            assert(1 == 0);
        }
#ifdef PUMP_HAS_CPP11
        if (!std::regex_match(value->asString(), m_dataPattern))
        {
            throw JVException(JVAL_ERR_FORMAT_MISMATCH, "Format error");
        }
#else
        regmatch_t matchs[1];
        if (regexec(&m_dataPattern, value->asString().c_str(), 1, matchs, 0) != 0)
        {
            throw JVException(JVAL_ERR_FORMAT_MISMATCH, "Format error");
        }
#endif // PUMP_HAS_CPP11
        return JVAL_ROK;
    }

    Enum::Enum(const Json::Value & value)
        : m_enum(value)
    {

    }

    int Enum::validate(const Json::Value *value)
    {
        for (unsigned int i = 0; i < m_enum.size(); ++i)
        {
            if (m_enum[i].asString() == value->asString())
            {
                return JVAL_ROK;
            }
        }
        throw JVException(JVAL_ERR_ENUMERATION_INVALID, "Enum error");
    }

    int Array::validate(const Json::Value *value)
    {
        if (!value->isArray()) {
            throw JVException(JVAL_ERR_NOT_AN_ARRAY, "Array error");
        }

        return JVAL_ROK;
    }

    /**
     * @brief Array validation keyword. Constructor
     */
    MinItems::MinItems(unsigned int minItems = 0)
    {
        m_minItems = minItems;
    }

    /**
     * @brief Array validation keyword, validates Minimum number of items present
     * in the list
     *
     * @param schema
     * @param value
     *
     * @return
     */
    int MinItems::validate(const Json::Value *value)
    {
        if (value->size() < m_minItems) {
            throw JVException(JVAL_ERR_INVALID_MIN_ITEMS, "MinItems error");
        }

        return JVAL_ROK;
    }

    /**
     * @brief Array validation keyword. Constructor
     */
    MaxItems::MaxItems(unsigned int maxItems = 0)
    {
        m_maxItems = maxItems;
    }

    /**
     * @brief Array validation keyword, validates maximum number of items present
     * in the list
     *
     * @param schema
     * @param value
     *
     * @return
     */
    int MaxItems::validate(const Json::Value *value)
    {
        if (value->size() > m_maxItems) {
            throw JVException(JVAL_ERR_INVALID_MAX_ITEMS, "MaxItems error");
        }

        return JVAL_ROK;
    }

    ItemsTuple::ItemsTuple(Json::Value items)
    {
        m_items = items;

        for (unsigned int i = 0; i < items.size(); i++) {
            m_primitives.push_back(JsonPrimitive::createPrimitive(&items[i]));
        }
    }

    ItemsTuple::~ItemsTuple()
    {
        for (unsigned int i = 0; i < m_primitives.size(); i++) {
            delete m_primitives[i];
        }
    }

    int ItemsTuple::validate(const Json::Value *value)
    {
        for (std::size_t i = 0; i < value->size() && i < m_primitives.size(); i++) {
            int ret = m_primitives[i]->validate(&((*value)[(int)i]));
            if (JVAL_ROK != ret) {
                throw JVException(JVAL_ERR_INVALID_ARRAY_ITEM, "ItemsTuple error");
            }
        }

        return JVAL_ROK;
    }

    ItemsList::ItemsList(Json::Value items)
    {
        m_items = items;
        m_primitive = JsonPrimitive::createPrimitive(&items);
        if (!m_primitive)
        {
            m_primitive = NULL;
            throw JVException(JVAL_ERR_SP_ITEMSLIST_INVALID, "Invalid ItemsList");
        }
    }

    ItemsList::~ItemsList()
    {
        delete m_primitive;
    }

    int ItemsList::validate(const Json::Value *value)
    {
        for (std::size_t i = 0; i < value->size(); i++) {
            try
            {
                m_primitive->validate(&((*value)[(int)i]));
            }
            catch (JsdValidator::JVException &e)
            {
                e.addMsgPre("ItemsList err");
                throw e;
            }
        }

        return JVAL_ROK;
    }

    /**
     * @brief Array Validation keyword. Constructor
     */
    UniqueItems::UniqueItems(bool uniqueItems = false)
    {
        m_uniqueItems = uniqueItems;
    }

    /**
     * @brief Array Validation keyword.
     *    Validates uniquieness in the list items
     *
     * @param schema
     * @param value
     *
     * @return
     */
    int UniqueItems::validate(const Json::Value *value)
    {
        std::set<Json::Value> valueSet;

        if (m_uniqueItems) {
            for (std::size_t i = 0; i < value->size(); i++) {
                std::pair<std::set<Json::Value>::iterator, bool> ret = \
                    valueSet.insert((*value)[(int)i]);
                if (false == ret.second) {
                    return JVAL_ERR_DUPLICATE_ITEMS;
                }
            }
        }

        return JVAL_ROK;
    }

    /**
     * @brief Array Validation keyword. Constructor
     */
    AdditionalItems::AdditionalItems(unsigned int itemsSize)
    {
        m_itemsSize = itemsSize;
    }

    /**
     * @brief Array Validation keyword. Validates additional items in a tuple.
     *
     * @param schema
     * @param value
     *
     * @return
     */
    int AdditionalItems::validate(const Json::Value *value)
    {
        // An empty array is always valid
        if (0 == value->size()) {
            return JVAL_ROK;
        }

        if (value->size() > m_itemsSize) {
            return JVAL_ERR_ADDITIONAL_ITEMS;
        }

        return JVAL_ROK;
    }

    /**
     * @brief Object validation keyword. Constructor
     */
    MaxProperties::MaxProperties(unsigned int maxProperties)
    {
        m_maxProperties = maxProperties;
    }

    int ObjectValid::validate(const Json::Value *value)
    {
        if (!value->isObject()) {
            return JVAL_ERR_NOT_AN_OBJECT;
        }

        return JVAL_ROK;
    }

    /**
     * @brief Object validation keyword. Validates maximum number of properties of
     * an object
     *
     * @param schema
     * @param value
     *
     * @return
     */
    int MaxProperties::validate(const Json::Value *value)
    {
        if (value->size() > m_maxProperties) {
            return JVAL_ERR_INVALID_MAX_PROPERTIES;
        }

        return JVAL_ROK;
    }

    /**
     * @brief Object validation keyword. Constructor
     */
    MinProperties::MinProperties(unsigned int minProperties = 0)
    {
        m_minProperties = minProperties;
    }

    /**
     * @brief Object validation keyword. Validates minimum number of properties of
     * an object
     *
     * @param schema
     * @param value
     *
     * @return
     */
    int MinProperties::validate(const Json::Value *value)
    {
        if (value->size() < m_minProperties) {
            return JVAL_ERR_INVALID_MIN_PROPERTIES;
        }

        return JVAL_ROK;
    }

    Required::Required(Json::Value required)
    {
        for (unsigned int i = 0; i < required.size(); i++) {
            if (!required[i].isString())
            {
                throw JVException(JVAL_ERR_SP_REQUIRED_INVALID, "Invalid Required");
            }
            m_required.push_back(required[i].asString());
        }
    }

    /**
     * @brief Object validation keyword. Validates mandatory properties of an
     * object
     *
     * @param schema
     * @param value
     *
     * @return
     */
    int Required::validate(const Json::Value *value)
    {
        for (size_t i = 0; i < m_required.size(); i++) {
            if (!value->isMember(m_required[i])) {
                return JVAL_ERR_REQUIRED_ITEM_MISSING;
            }
        }

        return JVAL_ROK;
    }

    Properties::Properties(Json::Value properties, bool additionalProperties = true)
    {
        m_properties = properties;
        m_additionalProperties = additionalProperties;
        bool bException = false;

        for (Json::ValueIterator itr = properties.begin(); \
            itr != properties.end(); \
            itr++) {

            Json::Value property = properties.get(itr.name(), property);
            JsonPrimitive *primitive = NULL;
            try
            {
                primitive = JsonPrimitive::createPrimitive(&property);
            }
            catch (JsdValidator::JVException &e)
            {
                destroy();
                std::string strMsg("Invalid Properties");
                strMsg += ("(" + itr.name() + "),");
                e.addMsgPre(strMsg.c_str());
                throw e;
            }
            m_primitives[itr.name()] = primitive;
        }
    }

    Properties::~Properties()
    {

    }

    void Properties::destroy()
    {
        for (std::map<std::string, JsonPrimitive*>::iterator itr = \
            m_primitives.begin(); itr != m_primitives.end();
            itr++)
        {
            delete itr->second;
        }
    }

    /**
     * @brief Object validation keyword. Validates properties of an object
     *
     * @param schema
     * @param value
     *
     * @return
     */
    int Properties::validate(const Json::Value *value)
    {
        if (m_additionalProperties) {
            return JVAL_ROK;
        }

        for (Json::ValueConstIterator itr = value->begin(); \
            itr != value->end(); \
            itr++) {
            JsonPrimitive *primitive = m_primitives[itr.name()];
            if (!primitive)
            {
                return JVAL_ERR_INVALID_PROPERTY;
            }
            Json::Value property = *itr;
            int ret = primitive->validate(&property);
            if (JVAL_ROK != ret) {
                return ret;
            }
        }

        return JVAL_ROK;
    }

    Dependencies::Dependencies(const Json::Value & val)
        : m_dep(val)
    {}

    int Dependencies::validate(const Json::Value *value)
    {
        for (unsigned int i = 0; i < m_dep.size(); ++i)
        {
            for (unsigned int j = 0; j < value->size(); ++j)
            {
                Json::Value t_val = m_dep.get(i);
                if (m_dep.name(i) == value->name(j))
                {
                    if (!t_val.isArray())
                    {
                        return JVAL_ERR_INVALID_SCHEMA;
                    }
                    for (unsigned int k = 0; k < t_val.size(); ++k)
                    {
                        bool bHave = false;
                        for (unsigned int s = 0; s < value->size(); ++s)
                        {
                            if (t_val[k].asString() == value->name(s))
                            {
                                bHave = true;
                            }
                        }
                        if (bHave == false)
                        {
                            return JVAL_ERR_DEPENDENCIES_INVALID;
                        }
                    }
                }
            }
        }
        return JVAL_ROK;
    }

    AdditionalProperties::AdditionalProperties()
    {
        m_additionalProperties = true;
    }

    int AdditionalProperties::validate(const Json::Value *value)
    {
        (void)value;
#if 0
        Json::Value additionalProps = schema->get("additionalProperties",
            additionalProps);
        if (additionalProps.isBool()) {
            m_additionalProperties = additionalProps.asBool();
            if (true == m_additionalProperties) {
                return JVAL_ROK;
            }
        } else if (additionalProps.isObject()) {
            return JVAL_ROK;
        } else {
            return JVAL_ERR_INVALID_SCHEMA;
        }
#endif

        return JVAL_ROK;
    }

}
