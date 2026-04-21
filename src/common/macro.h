#pragma once

#ifndef _MACRO_H
#define _MACRO_H

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) > (b) ? (b) : (a))

#ifndef _STDLIB_H 
#define	EXIT_FAILURE	1	/* Failing exit status.  */
#define	EXIT_SUCCESS	0	/* Successful exit status.  */
#endif

#ifndef PACKED
#define PACKED __attribute__ ((__packed__))
#endif

#ifdef EXPORT
// Exports
#	ifdef _MSC_VER
#		define API __declspec(dllexport)
#	else
#		define API __attribute__((visibility("default")))
#	endif
#else
// Imports
#	ifdef _MSC_VER
/** @brief Import/export qualifier */
#		define API __declspec(dllimport)
#	else
/** @brief Import/export qualifier */
#		define API
#	endif
#endif

#endif //_MACRO_H
