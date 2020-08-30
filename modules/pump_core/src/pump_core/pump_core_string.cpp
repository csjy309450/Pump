/**
 * @file pump_core_string.cpp
 * @brief Library Inner Implementation.
 * @details Implementation of string interface.
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

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "pump_core/pump_core_string.h"

void PUMP_CORE_Inner_PrintNum(unsigned long num, int base); ///< general number print 
void PUMP_CORE_Inner_PrintDeci(int dec);					///< print decimal number
void PUMP_CORE_Inner_PrintOct(unsigned oct);				///< print octonary number
void PUMP_CORE_Inner_PrintHex(unsigned hex);				///< print hexadecimal number
void PUMP_CORE_Inner_PrintAddr(unsigned long addr);			///< print address
void PUMP_CORE_Inner_PrintStr(char *str);					///< print string
void PUMP_CORE_Inner_PrintFloat(double f);					///< print float

PUMP_CORE_API pump_void_t PUMP_CALLBACK PUMP_CORE_Printf(const char* szFormate, ...)
{
    int i = 0;
    va_list va_ptr;
    va_start(va_ptr, szFormate);

    while (szFormate[i] != '\0')
    {
        if (szFormate[i] != '%')
        {
            putchar(szFormate[i++]);
            continue;
        }

        switch (szFormate[++i])
        {
        case 'd': PUMP_CORE_Inner_PrintDeci(va_arg(va_ptr, int));
            break;
        case 'o': PUMP_CORE_Inner_PrintOct(va_arg(va_ptr, unsigned int));
            break;
        case 'x': PUMP_CORE_Inner_PrintHex(va_arg(va_ptr, unsigned int));
            break;
        case 'c': putchar(va_arg(va_ptr, int));
            break;
        case 'p': PUMP_CORE_Inner_PrintAddr(va_arg(va_ptr, unsigned long));
            break;
        case 'f': PUMP_CORE_Inner_PrintFloat(va_arg(va_ptr, double));
            break;
        case 's': PUMP_CORE_Inner_PrintStr(va_arg(va_ptr, char *));
            break;
        default: break;
        }

        i++; 
    }

    va_end(va_ptr);
}

void PUMP_CORE_Inner_PrintNum(unsigned long num, int base)
{
    if (num == 0)
    {
        return;
    }

    PUMP_CORE_Inner_PrintNum(num / base, base);

    putchar("0123456789abcdef"[num%base]);
}

void PUMP_CORE_Inner_PrintDeci(int dec)
{
    if (dec < 0)
    {
        putchar('-');
        dec = -dec;  	   // FIXME exist overflow risk, minimum number have no corresponding positive number.
    }

    if (dec == 0)
    {
        putchar('0');
        return;
    }
    else
    {
        PUMP_CORE_Inner_PrintNum(dec, 10); // print deci number
    }
}

void PUMP_CORE_Inner_PrintOct(unsigned oct)
{
    if (oct == 0)
    {
        putchar('0');
        return;
    }
    else
    {
        PUMP_CORE_Inner_PrintNum(oct, 8);
    }
}

void PUMP_CORE_Inner_PrintHex(unsigned hex)
{
    if (hex == 0)
    {
        putchar('0');
        return;
    }
    else
    {
        PUMP_CORE_Inner_PrintNum(hex, 16);
    }
}

void PUMP_CORE_Inner_PrintAddr(unsigned long addr)
{
    /* print "0x" */
    putchar('0');
    putchar('x');
    PUMP_CORE_Inner_PrintNum(addr, 16);
}

void PUMP_CORE_Inner_PrintStr(char *str)
{
    int i = 0;

    while (str[i] != '\0')
    {
        putchar(str[i++]);
    }
}

void PUMP_CORE_Inner_PrintFloat(double f)
{
    int temp;

    // print integer part
    temp = (int)f;
    PUMP_CORE_Inner_PrintNum(temp, 10);
    putchar('.');
    // print float part
    f -= temp;
    if (f == 0)
    {
        /* six precision */
        for (temp = 0; temp < 6; temp++)
        {
            putchar('0');
        }
        return;
    }
    else
    {
        temp = (int)(f * 1000000);
        PUMP_CORE_Inner_PrintNum(temp, 10);
    }
}

#ifdef PUMP_COMPILER_CXX

PUMP_CORE_CXXAPI std::string PUMP_CALLBACK PUMP_CORE_StringTrim(const std::string & strSrc)
{
    if (strSrc.empty())
    {
        return strSrc;
    }
    std::string strBuff = strSrc;
    for (std::string::reverse_iterator it = strBuff.rbegin(); it != strBuff.rend(); it = strBuff.rbegin())
    {
        if ((*it) == ' '
            || (*it) == '\n'
            || (*it) == '\r'
            || (*it) == '\t')
        {
            //strBuff.pop_back();
            strBuff.erase(strBuff.end());
        }
        else
        {
            return strBuff;
        }
    }
    return strBuff;
}

PUMP_CORE_CXXAPI std::string PUMP_CALLBACK PUMP_CORE_StringReplase(const std::string &strSrc, const std::string &pattern, const std::string &target)
{
    std::string strBuff = strSrc;
    std::string::size_type pos = 0;
    std::string::size_type pattern_size = pattern.size();
    std::string::size_type target_size = target.size();
    while ((pos = strBuff.find(pattern, pos)) != std::string::npos)
    {
        strBuff.replace(pos, pattern_size, target);
        pos += target_size;
    }
    return strBuff;
}

PUMP_CORE_CXXAPI std::string PUMP_CALLBACK PUMP_CORE_Sprintf(const pump_str_t szFormate, ...)
{
    std::string strDist;
    int i = 0;
    va_list va_ptr;
    va_start(va_ptr, szFormate);

    while (szFormate[i] != '\0')
    {
        if (szFormate[i] != '%')
        {
            strDist.append(1, szFormate[i++]);
            continue;
        }

        switch (szFormate[++i]) 
        {
        case 'd':
        {
            int val = va_arg(va_ptr, int);
            strDist += PUMP_CORE_Other2Str(val);
        }

        break;
        case 'o':
        {
            unsigned int val = va_arg(va_ptr, unsigned int);
            strDist += PUMP_CORE_Other2Str(val);
        }

        break;
        case 'x':
        {
            unsigned int val = va_arg(va_ptr, unsigned int);
            strDist += PUMP_CORE_Other2Str(val);
        }

        break;
        case 'c':
        {
            int val = va_arg(va_ptr, int);
            strDist += PUMP_CORE_Other2Str(val);
        }
        break;
        case 'p':
        {
            unsigned long val = va_arg(va_ptr, unsigned long);
            strDist += PUMP_CORE_Other2Str(val);
        }
        break;
        case 'f':
        {
            double val = va_arg(va_ptr, double);
            strDist += PUMP_CORE_Other2Str(val);
        }
        break;
        case 's':
        {
            const char * pStr = va_arg(va_ptr, const char *);
            strDist += pStr;
        }
        break;
        default: break;
        }

        i++;
    }

    va_end(va_ptr);
    return strDist;
}

PUMP_CORE_CXXAPI std::string PUMP_CALLBACK PUMP_CORE_Vprintf(const pump_str_t szFormate, va_list argv)
{
    std::string strDist;
    //int i = 0;

    size_t  dwLen = strlen(szFormate);

    for (size_t i = 0; i < dwLen; ++i)
    {
        if (szFormate[i] != '%')
        {
            strDist.append(1, szFormate[i]);
            continue;
        }
        if (i >= dwLen - 1)
        {
            continue;
        }

        switch (szFormate[i + 1])
        {
        case 'd':
        {
            int val = va_arg(argv, int);
            strDist += PUMP_CORE_Other2Str(val);
        }
        break;
        case 'o':
        {
            unsigned int val = va_arg(argv, unsigned int);
            strDist += PUMP_CORE_Other2Str(val);
        }
        break;
        case 'x':
        {
            unsigned int val = va_arg(argv, unsigned int);
            strDist += PUMP_CORE_Other2Str(val);
        }
        break;
        case 'u':
        {
            unsigned int val = va_arg(argv, unsigned int);
            strDist += PUMP_CORE_Other2Str(val);
        }
        break;
        case 'c':
        {
            int val = va_arg(argv, int);
            strDist += PUMP_CORE_Other2Str(val);
        }
        break;
        case 'p':
        {
            unsigned long val = va_arg(argv, unsigned long);
            strDist += PUMP_CORE_Other2Str(val);
        }
        break;
        case 'f':
        {
            double val = va_arg(argv, double);
            strDist += PUMP_CORE_Other2Str(val);
        }
        break;
        case 's':
            strDist += va_arg(argv, const char *);
            break;
        case 'l':
        {
            if (i + 1 >= dwLen - 1)
            {
                continue;
            }
            switch (szFormate[i + 2])
            {
            case 'd':
            {
                long val = va_arg(argv, long);
                strDist += PUMP_CORE_Other2Str(val);
                ++i;
            }
            default:
                break;
            }
        }
        break;
        default:
            break;
        }
        ++i;
    }

    va_end(argv);
    return strDist;
}

#endif // PUMP_COMPILER_CXX