/*!
* @file AC.hpp
* @brief header of Auto Converter.
* 
* This file contains the class elements of the Auto Converter.
*
* @author yangzheng
* @version 1.0
* @date 2019-10-07
*/
#ifdef WIN32
#pragma once
#endif // WIN32
#ifndef _AC_HPP_
#define _AC_HPP_
#include <cstdio>
#include <string>
#include <vector>
#include <sstream>
#include <memory>
#include "pump_ac/AC_Types.h"
#include "pump_ac/AC_Utils.hpp"

/** Get reference of ac's member by name. */
#define GET(ac,type,name) \
	(*(type*)(ac).get(name))

/** Get pointer of ac's member by name. */
#define GETPTR(ac,type,name) \
	((type*)(ac).get(name))

/** Get pointer of ac's member by index. */
#define GET_BY_INDEX(ac,type,index) \
	*(type*)ac.get_by_index(index)

enum AC_Type
{
	AC_DEFAUT,
	AC_VALUE,
	AC_COMPOUND,
};

class AutoConverter;
class Serializer;
template<class ACType> class AC_Container;
class AC_Struct;
class AC_Array;
class AC_NumberArray;
class AC_Enum;
template<class T> class AC_Variable;
template<class T> class AC_Number;
typedef AC_Number<int> AC_Int;
typedef AC_Number<double> AC_Float;
class AC_Char;
class AC_String;
class AC_Boolean;
class AC_2DPoint;

std::string AC_To_Json(AutoConverter & ac);

std::string AC_To_Json(std::vector<std::shared_ptr<AutoConverter>> & ac);

/**
 * @brief interface of object serializer
 */
class Serializer
{
public:
	virtual std::string Serialize(AutoConverter * pAc) = 0;
};

/**
* @brief abstract class for converting c object to text(json/xml)
*/
class AutoConverter
{
public:
	AutoConverter(const char* name, std::shared_ptr<Serializer> pSerial, AC_Type emType)
		: m_name(name), m_pSerial(pSerial), m_emType(emType) {}
	virtual ~AutoConverter() { (m_pSerial ? m_pSerial.reset() : (NULL)); }
	virtual std::string Serialize() { return (m_pSerial ? m_pSerial->Serialize(this) : std::string()); }

	virtual void * get(const char* name){ return NULL; }
	const AC_Type type(){ return m_emType; }
	virtual bool empty() = 0;
	virtual void reset() = 0;
public:
	const char* m_name;
	std::shared_ptr<Serializer> m_pSerial;
	const AC_Type m_emType;

protected:
	AutoConverter() : m_name(NULL), m_pSerial(NULL), m_emType(AC_DEFAUT){}
};

/**
* @brief AutoConverter object container
*/
template<class T>
class AC_Container
{
private:
#define AC_Mem(_type, _argv) \
	AC_MemType(new _type _argv)
public:
	typedef std::shared_ptr<T> AC_MemType;
	typedef std::vector<std::shared_ptr<T>> AC_MemContainerType;

	AC_Container(){}
	AC_Container(const AC_MemContainerType & vec_mems)
		: m_val(vec_mems){}
	AC_Container(const AC_Container & other)
		: m_val(other.m_val){}
	virtual ~AC_Container()
	{
		for (AC_MemContainerType::iterator it = m_val.begin();
			it != m_val.end(); ++it)
		{
			if ((*it))
				(*it).reset();
		}
	}

	virtual void * get_by_name(const char* name)
	{
		if (!name)
		{
			return NULL;
		}
		for (AC_MemContainerType::iterator it = m_val.begin();
			it != m_val.end(); ++it)
		{
			if (!strcmp((*it).get()->m_name, name))
			{
				return (*it).get();
			}
		}
		return NULL;
	}

	virtual void * get_by_index(unsigned int indx)
	{
		if (indx >= m_val.size())
		{
			return NULL;
		}
		return m_val[indx].get();
	}

	virtual void push_back(AC_MemType ac)
	{
		m_val.push_back(ac);
	}

	virtual void pop_back()
	{
		m_val.pop_back();
	}

	virtual typename AC_MemContainerType::reference front()
	{
		return m_val.front();
	}

	virtual void insert(typename AC_MemContainerType::const_iterator where, AC_MemType ac)
	{
		m_val.insert(where, ac);
	}

	virtual void erase(typename AC_MemContainerType::const_iterator where)
	{
		m_val.erase(where);
	}
protected:
	AC_MemContainerType m_val;
};

/**
* @brief simulate to python struct type, user could add member dynamically.
*/
class AC_Struct : public AutoConverter, public AC_Container<AutoConverter>
{
private:
	typedef class SerialJsonStruct : public Serializer
	{
	public:
		virtual std::string Serialize(AutoConverter *  pAc)
		{
			std::string strOut;
			if (pAc == NULL || pAc->empty())
			{
				return "{}";
			}
			AC_Struct * pCom = static_cast<AC_Struct*>(pAc);
			if (pCom->m_name == NULL)
			{
				return strOut;
			}
			//if (!strcmp(pCom->m_name, ""))
			//{
			//	strOut = "{";
			//}
			//else
			//{
			//	strOut = "\"" + std::string(pCom->m_name) + "\":{";
			//}
			strOut = "{";
			bool bFirstItem = true;
			for (AC_MemContainerType::iterator it = pCom->m_val.begin();
				it != pCom->m_val.end(); ++it)
			{
				if ((*it) && !(*it)->empty())
				{
					strOut += (bFirstItem ? "" : ",");
					strOut += "\"" + std::string((*it)->m_name) + "\":" + (*it)->Serialize();
					if (bFirstItem)
					{
						bFirstItem = false;
					}
				}
			}
			strOut += "}";

			return strOut;
		}
	} SerializerType;
public:
	typedef std::shared_ptr<AutoConverter> AC_MemType;
	typedef std::vector<std::shared_ptr<AutoConverter>> AC_MemContainerType;

	AC_Struct(const char * name, PlaceHolder place_holder = PLACE_HOLDER)
		: AutoConverter(name, std::shared_ptr<SerializerType>(new SerializerType()), AC_COMPOUND){}

	AC_Struct(const char * name, const AC_MemContainerType & vec_mems)
		: AutoConverter(name, std::shared_ptr<SerializerType>(new SerializerType()), AC_COMPOUND)
		, AC_Container(vec_mems){}

	AC_Struct(const AC_Struct & other)
		: AutoConverter(other.m_name, std::shared_ptr<SerializerType>(new SerializerType()), AC_COMPOUND)
		, AC_Container(other.m_val){}

	virtual ~AC_Struct()
	{
		for (AC_MemContainerType::iterator it = m_val.begin();
			it != m_val.end(); ++it)
		{
			if ((*it))
				(*it).reset();
		}
	}

	virtual void * get(const char* name)
	{
		return get_by_name(name);
	}
	virtual bool empty()
	{
		return m_val.empty();
	}
	virtual void reset()
	{
		m_val.clear();
	}
	size_t size()
	{
		return m_val.size();
	}
protected:
	AC_Struct() : AutoConverter(NULL, NULL, AC_DEFAUT){}
};

/**
* @brief simulate to java List type, user could add member dynamically.
*/
class AC_Array : public AutoConverter, public AC_Container<AutoConverter>
{
private:
	typedef class SerialJsonArray : public Serializer
	{
	public:
		virtual std::string Serialize(AutoConverter *  pAc)
		{
			std::string strOut, strPrefix, strPostfix;
			if (pAc == NULL || pAc->empty())
			{
				return "[]";
			}
			AC_Array * pCom = static_cast<AC_Array*>(pAc);
			if (pCom->m_val.empty())
			{
				return "[]";
			}
			//if (pCom->m_name == NULL)
			//{
			//	strOut = "[";
			//}
			//else
			//{
			//	strOut = "\"" + std::string(pCom->m_name) + "\":[";
			//}

			strOut = "[";
			bool bFirstItem = true;
			for (AC_MemContainerType::iterator it = pCom->m_val.begin();
				it != pCom->m_val.end(); ++it)
			{
				if ((*it) && !(*it)->empty())
				{
					strOut += strPrefix + (bFirstItem ? "" : ",");
					strOut += (*it)->Serialize() + strPostfix;
					if (bFirstItem)
					{
						bFirstItem = false;
					}
				}
			}
			strOut += ("]");
			return strOut;
		}
	}SerializerType;

public:
	AC_Array(const char * name, PlaceHolder place_holder)
		: AutoConverter(name, std::shared_ptr<SerializerType>(new SerializerType()), AC_COMPOUND){}

	AC_Array(const char * name, const AC_MemContainerType & vec_mems)
		: AutoConverter(name, std::shared_ptr<SerializerType>(new SerializerType()), AC_COMPOUND)
		, AC_Container(vec_mems){}

	AC_Array(const char * name, const AC_Array & other)
		: AutoConverter(name, std::shared_ptr<SerializerType>(new SerializerType()), AC_COMPOUND)
		, AC_Container(other.m_val){}

	virtual ~AC_Array()
	{
		for (AC_MemContainerType::iterator it = m_val.begin();
			it != m_val.end(); ++it)
		{
			if ((*it))
				(*it).reset();
		}
	}
	virtual void * get(const char* name)
	{
		return get_by_name(name);
	}
	virtual void * get_by_index(unsigned int indx)
	{
		return AC_Container::get_by_index(indx);
	}
	virtual bool empty()
	{
		return m_val.empty();
	}
	virtual void reset()
	{
		m_val.clear();
	}
protected:
	AC_Array() : AutoConverter(NULL, NULL, AC_DEFAUT){}
};

/**
* @brief enum type.
*/
class AC_Enum : public AutoConverter, public AC_Container<AutoConverter>
{
private:
	typedef class SerialJsonEnum : public Serializer
	{
	public:
		virtual std::string Serialize(AutoConverter *  pAc)
		{
			std::string strOut;
			if (pAc == NULL || pAc->empty())
			{
				return strOut;
			}
			AC_Enum * pEnum = static_cast<AC_Enum*>(pAc);
			if ((size_t)pEnum->m_index < pEnum->m_val.size())
			{
				//strOut = ((pEnum->m_name != NULL) ? "\"" + std::string(pEnum->m_name) + "\":" : "") + pEnum->m_val[pEnum->m_index]->Serialize();
				strOut = pEnum->m_val[pEnum->m_index]->Serialize();
			}
			return strOut;
		}
	}SerializerType;
public:
	AC_Enum(const char* name, unsigned int indx)
		: AutoConverter(name, std::shared_ptr<SerialJsonEnum>(new SerialJsonEnum()), AC_COMPOUND), m_index(indx){}
	AC_Enum(const char* name, PlaceHolder placeHolder)
		: AutoConverter(name, std::shared_ptr<SerialJsonEnum>(new SerialJsonEnum()), AC_COMPOUND), m_index(-1){}
	~AC_Enum() {}
	virtual bool empty()
	{
		return (m_index == -1);
	}
	virtual void reset()
	{
		m_index = -1;
	}
protected:
	int m_index;
};

/**
* @brief base variable type.
*/
template<class T>
class AC_Variable : public AutoConverter
{
public:
	AC_Variable(const char* name, T val, std::shared_ptr<Serializer> pSerial)
		: AutoConverter(name, pSerial, AC_VALUE), m_val(val), m_bEmpty(false){}
	AC_Variable(const char* name, PlaceHolder placeHolder, std::shared_ptr<Serializer> pSerial)
		: AutoConverter(name, pSerial, AC_VALUE), m_bEmpty(true){}
	virtual ~AC_Variable() {}
	virtual void * get(const char* name) { return &m_val; }
	virtual bool empty()
	{
		return m_bEmpty;
	}
	virtual void reset()
	{
		m_bEmpty = true;
	}
protected:
	T m_val;
	bool m_bEmpty;
};

/**
* @brief numerical variable type.
*/
template<class T>
class  AC_Number : public AC_Variable<T>
{
private:
	typedef class SerialJsonNumber : public Serializer
	{
	public:
		virtual std::string Serialize(AutoConverter * pAc)
		{
			std::string strOut;
			if (pAc == NULL || pAc->empty())
			{
				return strOut;
			}
			AC_Number * pNember = static_cast<AC_Number*>(pAc);
			//strOut = (pNember->m_name == NULL ? "" : "\"" + std::string(pNember->m_name) + "\":") + number2str<T>(pNember->m_val);
			strOut = number2str<T>(pNember->m_val);
			return strOut;
		}
	}SerializerType;
public:
	AC_Number(AC_Number & other)
		: AC_Variable<T>(other.m_name, other.m_val, std::shared_ptr<SerializerType>(new SerializerType())) {}
	AC_Number(const char* name, T val)
		: AC_Variable<T>(name, val, std::shared_ptr<SerializerType>(new SerializerType())){}
	AC_Number(const char* name, PlaceHolder placeHolder)
		: AC_Variable<T>(name, placeHolder, std::shared_ptr<SerializerType>(new SerializerType())){}
	virtual ~AC_Number() {}
};

class AC_Function : public AC_Variable<std::string>
{
private:
	typedef class SerialJsonFunction : public Serializer
	{
	public:
		virtual std::string Serialize(AutoConverter * pAc)
		{
			std::string strOut;
			if (pAc == NULL || pAc->empty())
			{
				return strOut;
			}
			AC_Function * pNember = static_cast<AC_Function*>(pAc);
			//strOut = (pNember->m_name == NULL ? "" : "\"" + std::string(pNember->m_name) + "\":") + pNember->m_val;
			strOut = pNember->m_val;
			return strOut;
		}
	} SerializerType;
public:
	AC_Function(const char* name, std::string & val)
		: AC_Variable<std::string>(name, val, std::shared_ptr<SerializerType>(new SerializerType())){}
	AC_Function(const char* name, PlaceHolder placeHolder)
		: AC_Variable<std::string>(name, placeHolder, std::shared_ptr<SerializerType>(new SerializerType())){}
	virtual ~AC_Function() {}
};

class AC_NumberArray : public AC_Array
{
public:
	AC_NumberArray(const char * name, PlaceHolder place_holder) : AC_Array(name, place_holder) {}
	AC_NumberArray(const char * name, const AC_Array & other) : AC_Array(name, other) {}

	void push_back(int32_t x)
	{
		AC_Array::push_back(AC_Mem(AC_Int, (NULL, x)));
	}

	void push_back(float64_t x)
	{
		AC_Array::push_back(AC_Mem(AC_Float, (NULL, x)));
	}
};

class AC_Char : public AC_Variable<char>
{
private:
	typedef class SerialJsonChar : public Serializer
	{
	public:
		virtual std::string Serialize(AutoConverter * pAc)
		{
			std::string strOut;
			if (pAc == NULL || pAc->empty())
			{
				return strOut;
			}
			AC_Char * pNember = static_cast<AC_Char*>(pAc);
			//strOut = (pNember->m_name == NULL ? "" : "\"" + std::string(pNember->m_name) + "\":\"") + pNember->m_val + "\"";
			strOut += "\"";
			strOut += pNember->m_val;
			strOut += "\"";
			return strOut;
		}
	}SerializerType;
public:
	AC_Char(const char* name, char val)
		: AC_Variable<char>(name, val, std::shared_ptr<SerializerType>(new SerializerType())){}
	AC_Char(const char* name, PlaceHolder placeHolder)
		: AC_Variable<char>(name, placeHolder, std::shared_ptr<SerializerType>(new SerializerType())){}
	virtual ~AC_Char() {}
};

class AC_String : public AC_Variable<std::string>
{
private:
	typedef class SerialJsonString : public Serializer
	{
	public:
		virtual std::string Serialize(AutoConverter * pAc)
		{
			std::string strOut;
			if (pAc == NULL || pAc->empty())
			{
				return strOut;
			}
			AC_String * pString = static_cast<AC_String*>(pAc);
			//strOut = (pString->m_name == NULL ? "" : "\"" + std::string(pString->m_name) + "\":") + "\"" + pString->m_val + "\"";
			strOut = "\"" + pString->m_val + "\"";
			return strOut;
		}
	}SerializerType;
public:
	AC_String(const char* name, const std::string & val)
		: AC_Variable<std::string>(name, val, std::shared_ptr<SerializerType>(new SerializerType())){}
	AC_String(const char* name, PlaceHolder placeHolder)
		: AC_Variable<std::string>(name, placeHolder, std::shared_ptr<SerializerType>(new SerializerType())){}
	virtual bool empty()
	{
		return m_val.empty();
	}
	virtual void reset()
	{
		m_val.clear();
	}
	virtual ~AC_String() {}
};

class AC_Boolean : public AC_Variable<bool>
{
private:
	typedef class SerialJsonBoolean : public Serializer
	{
	public:
		virtual std::string Serialize(AutoConverter * pAc)
		{
			std::string strOut;
			if (pAc == NULL || pAc->empty())
			{
				return strOut;
			}
			AC_Boolean * pNember = static_cast<AC_Boolean*>(pAc);
			//strOut = (pNember->m_name == NULL ? "" : "\"" + std::string(pNember->m_name) + "\":") + (pNember->m_val ? std::string("true") : std::string("false")) + "";
			strOut = (pNember->m_val ? std::string("true") : std::string("false")) + "";
			return strOut;
		}
	}SerializerType;
public:
	AC_Boolean(const char* name, bool val)
		: AC_Variable<bool>(name, val, std::shared_ptr<SerializerType>(new SerializerType())){}
	AC_Boolean(const char* name, PlaceHolder placeHolder)
		: AC_Variable<bool>(name, placeHolder, std::shared_ptr<SerializerType>(new SerializerType())){}
	virtual ~AC_Boolean() {}
};

class AC_2DPoint : public AC_Array
{
public:
	AC_2DPoint(const char * name, PlaceHolder place_holder)
		: AC_Array(name, place_holder)
	{}

	AC_2DPoint(const char * name, int arr[2])
		: AC_Array(name, PLACE_HOLDER)
	{
		m_val =
		{
			AC_Mem(AC_Int, (NULL, arr[0])),
			AC_Mem(AC_Int, (NULL, arr[1])),
		};
	}

	virtual void push_back(AC_MemType ac)
	{
		if (m_val.size()<2)
			m_val.push_back(ac);
	}

	~AC_2DPoint() {}
};

std::string AC_To_Json(AutoConverter & ac)
{
    return ac.Serialize();
}

std::string AC_To_Json(std::vector<std::shared_ptr<AutoConverter>> & ac)
{
    std::string strJson;
    for (std::vector<std::shared_ptr<AutoConverter>>::iterator it = ac.begin();
        it != ac.end(); ++it)
    {
        strJson += (*it)->Serialize() + ((it + 1) != ac.end() ? "," : "");
    }
    return "{" + strJson + "}";
}

#endif // !_AC_HPP_