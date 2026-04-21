#pragma once

#ifndef _PLATFORM_H
#define _PLATFORM_H

// Platform detection
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
    #define PLATFORM_WINDOWS 1
    #ifndef _WIN64
        #error "64-bit is required on Windows!"
    #endif
#elif defined(__linux__) || defined(__gnu_linux__)
// Linux OS
#	define PLATFORM_LINUX 1
#	if defined(__ANDROID__)
    #define PLATFORM_ANDROID 1
#	endif
#elif defined(__unix__)
    // Catch anything not caught by the above.
    #define PLATFORM_UNIX 1
#else
    #error "Unknown platform!"
#endif

#endif //_PLATFORM_H
