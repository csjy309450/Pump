/*!
* \file AC_Utiles.h
* \brief Header of tool functions.
*
*This file contains tool functions.
*
* \author yangzheng
* \version 1.0
* \date 2019-10-07
*/
#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>
#include <sstream>

#ifdef AUTOCONVERTER_EXPORTS
#define AC_EXP_CLASS _declspec( dllexport )
#define AC_EXP_API  __declspec(dllexport)
#else
#define AC_EXP_CLASS _declspec(dllimport)
#define AC_EXP_API  __declspec(dllimport)
#endif

typedef struct tagPlaceHolder {} PlaceHolder, *LPPlaceHolder;

#define PLACE_HOLDER PlaceHolder()

template<typename T>
T str2number(std::string sznum, const std::string type = "dec"){
	std::stringstream ss;
	T temp;
	ss << sznum;
	if (type == "hex")
		ss >> std::hex >> temp;
	else if (type == "dec")
		ss >> std::dec >> temp;
	else if (type == "oct")
		ss >> std::oct >> temp;
	return temp;
}

template<typename T>
std::string number2str(T num, const std::string type = "dec"){
	std::stringstream ss;
	std::string temp;

	if (type == "hex")
		ss << std::hex << num;
	else if (type == "dec")
		ss << std::dec << num;
	else if (type == "oct")
		ss << std::oct << num;
	ss >> temp;
	return temp;
}

int replace_all(std::string & src, const char* parten, const char* dist)
{
    if (parten == NULL)
    {
        return -1;
    }
    if (src.empty())
    {
        return -1;
    }
    do
    {
        size_t pos = src.find(parten); // 查找每一行中的"Tom"
        if (pos != std::string::npos)
        {
            src = src.replace(pos, strlen(parten), dist); // 将Tom替换为Jerry
            continue;
        }
        else
        {
            break;
        }
    } while (1);
    return 0;
}

#endif // !_UTILS_H_
