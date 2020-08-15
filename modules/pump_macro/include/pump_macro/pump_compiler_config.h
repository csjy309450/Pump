#ifndef PUMP_COMPILER_CONFIG_H
#define PUMP_COMPILER_CONFIG_H
#include "pump_macro/pump_os_config.h"

#ifdef PUMP_OS_WINDOWS
#   if (defined _MSC_VER)
#       define PUMP_HAS_MSVC 
#       define PUMP_MSVC_VER _MSC_VER
#       define PUMP_MSVC_LANG _MSVC_LANG
#   elif (defined __GNUC__)
#       define PUMP_HAS_GNUC 1
#       define PUMP_GNUC_VER 
#       define PUMP_GNUC_LANG __cplusplus
#   endif // _MSC_VER

#   define PUMP_MSC_VER _MSC_VER
#   if (PUMP_MSC_VER >= 1700) //vs2012
#       pragma warning(disable: 4996)
#       define PUMP_MSVC_2012
#   elif (PUMP_MSC_VER >= 1600)
#       pragma warning(disable: 4996)
#       define PUMP_MSVC_2010
#   elif (PUMP_MSC_VER >= 1500)
#       pragma warning(disable: 4996)
#       define PUMP_MSVC_2008
#   elif (PUMP_MSC_VER >= 1400)
#       define PUMP_MSVC_2005
#   elif (PUMP_MSC_VER >= 1310)  //vs2003
#       define PUMP_MSVC_2003
#       define _WIN32_WINNT 0x0500
#   else
#       define PUMP_MSVC_1998
#       define _WIN32_WINNT 0x0500
#       pragma warning(disable: 4786)
#       pragma warning(disable: 4710)
#   endif // (PUMP_MSC_VER >= 1700)

/**
 * The version of C++ standard that is being used.
 * The C++17 standard.
 */
#   ifndef PUMP_HAS_CPP17
#	    ifdef PUMP_HAS_MSVC
#		    if PUMP_MSVC_LANG > 201402L
#			    define PUMP_HAS_CPP17	1
#		    else /* PUMP_MSVC_LANG > 201402L */
#			    define PUMP_HAS_CPP17	0
#		    endif /* PUMP_MSVC_LANG > 201402L */
#	    elif (defined PUMP_HAS_GNUC) 
#		    if PUMP_GNUC_LANG > 201402L
#			    define PUMP_HAS_CPP17	1
#		    else /* PUMP_GNUC_LANG > 201402L */
#			    define PUMP_HAS_CPP17	0
#		    endif /* PUMP_GNUC_LANG > 201402L */
#       endif /* PUMP_HAS_MSVC */
#   endif /* PUMP_HAS_CPP17 */

/**
 * The version of C++ standard that is being used.
 * The C++14 standard.
 */
#   ifndef PUMP_HAS_CPP14
#	    ifdef PUMP_HAS_MSVC
#		    if PUMP_MSVC_LANG > 201103L
#		    	define PUMP_HAS_CPP14	1
#		    else /* PUMP_MSVC_LANG > 201402L */
#		    	define PUMP_HAS_CPP14	0
#	    	endif /* PUMP_MSVC_LANG > 201402L */
#	    elif (defined PUMP_HAS_GNUC) 
#		    if PUMP_GNUC_LANG > 201103L
#			    define PUMP_HAS_CPP14	1
#		    else /* PUMP_GNUC_LANG > 201402L */
#	    		define PUMP_HAS_CPP14	0
#	    	endif /* PUMP_GNUC_LANG > 201402L */
#       endif /* PUMP_HAS_MSVC */
#   endif /* PUMP_HAS_CPP14 */

/**
 * The version of C++ standard that is being used.
 * The C++11 standard.
 */
#   ifndef PUMP_HAS_CPP11
#	    ifdef PUMP_HAS_MSVC
#		    if PUMP_MSVC_LANG > 199711L
#		    	define PUMP_HAS_CPP11	1
#		    else /* PUMP_MSVC_LANG > 199711L */
#		    	define PUMP_HAS_CPP11	0
#		    endif /* PUMP_MSVC_LANG > 199711L */
#	    elif (defined PUMP_HAS_GNUC) 
#	    	if PUMP_GNUC_LANG > 199711L
#			    define PUMP_HAS_CPP11	1
#		    else /* PUMP_GNUC_LANG > 199711L */
#			    define PUMP_HAS_CPP11	0
#		    endif /* PUMP_GNUC_LANG > 199711L */
#       endif /* PUMP_HAS_MSVC */
#   endif /* PUMP_HAS_CPP11 */
#endif // PUMP_OS_WINDOWS

#if (defined PUMP_OS_WINDOWS) && (defined PUMP_MSC_VER)
#   pragma warning(disable: 4251)  /* using non-exported as public in exported */
#endif // PUMP_OS_WINDOWS

#endif // PUMP_COMPILER_CONFIG_H