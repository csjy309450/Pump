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

#ifndef __VALIDATOR_H__
#define __VALIDATOR_H__

#include "pump_ac/jsoncpp/primitive_base.h"
namespace JsdValidator
{

    class JsonValidator
    {
    public:
        JsonValidator();

        JsonValidator(Json::Value *schema);

        JsonValidator(std::string &schema);

        /**
         * @brief Reads json schema from a file
         *
         * @param schema_file
         */
        void readSchema(const char *schema_file);
        void parseSchema(std::string &str);
        int validate(const Json::Value *value);
        Json::Value * getSchema();

        ~JsonValidator();

    private:
        Json::Value * m_schema;
        JsonPrimitive *m_primitive;
    };

}

#endif
