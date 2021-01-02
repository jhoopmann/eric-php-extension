/*
 * Copyright © LfSt Bayern, Muenchen, IuK 111, 2006-2012
 */

#ifndef ERIC_PLATFORM_H
#define ERIC_PLATFORM_H

/**
 * @file
 * @brief Konstanten für verschiedene Betriebssysteme
 */



/* Definitionen für die Eigenschaften der unterstützten Prozessorarchitekturen */

/* AMD64 */
#if defined(__amd64__) || defined(__x86_64__) || defined(__amd64) || defined(__x86_64) || defined(_M_AMD64) || defined(_M_X64)
#   define ARCH_CPU_X86
#   define ARCH_BIT_64
#   define ARCH_AMD64

/* Intel x86 */
#elif defined(i386) || defined(__i386) || defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__) || \
      defined(_M_IX86) || defined(__X86__)  || defined(_X86_) || defined(__I86__) || defined(__INTEL__) || defined(__THW_INTEL__)
#   define ARCH_CPU_X86
#   define ARCH_BIT_32
#   ifndef __i386__
#       define __i386__ 1
#   endif

/* ARM 64-Bit */
#elif defined(__aarch64__)
#   define ARCH_CPU_ARM
#   define ARCH_BIT_64

/* ARM 32-Bit */
#elif defined(__arm__) || defined(_ARM) || defined(_M_ARM) || defined (__arm) || defined(__TARGET_ARCH_ARM)
#   define ARCH_CPU_ARM
#   define ARCH_BIT_32

/* Power 64-Bit */
#elif defined(_ARCH_PPC64) || defined(__PPC64__) || defined(__powerpc64__)
#   define ARCH_CPU_POWER
#   define ARCH_BIT_64

/* Nicht unterstützte Architekturen */
#else
#   error Archtitektur wird nicht unterstuetzt!
#endif



/* macOS, iOS */

#if defined(__APPLE__)
    #include "TargetConditionals.h"
    #if TARGET_IPHONE_SIMULATOR
		#define __IOS_SIMULATOR__
		#define __IOS__
		#define I_OS

		#define FILENAMES_CASE_SENSITIVE 0
		#define strcasecmp strcmp

    #elif TARGET_OS_IPHONE
		#define __IOS_DEVICE__
		#define __IOS__
		#define I_OS

		#define FILENAMES_CASE_SENSITIVE 0
		#define strcasecmp strcmp

    #elif TARGET_OS_MAC
		#define OS_X
		#define MAC_OS_X
		#define MAC_OS
		#ifndef PLATFORM_NAME
		#   define PLATFORM_NAME "Mac OS X"
		#endif

		#define FILENAMES_CASE_SENSITIVE 0
		#define strcasecmp strcmp
    #endif
#endif


/* Windows */

#if defined(WINDOWS) || defined(_WIN32) || defined(_MSC_VER)
#   define WINDOWS_OS
#   ifndef PLATFORM_NAME
#       define PLATFORM_NAME "Windows"
#   endif
#endif


/* Linux */

#if defined(linux) || defined(__linux__)
#   define LINUX_OS

#   ifndef PLATFORM_NAME
#       if defined(ARCH_CPU_X86) && defined(ARCH_BIT_32)
#           define PLATFORM_NAME "Linux/x86"
#       elif defined(ARCH_CPU_X86) && defined(ARCH_BIT_64)
#           define PLATFORM_NAME "Linux/x86-64"
#       elif defined(ARCH_CPU_POWER)
#           define PLATFORM_NAME "Linux/PPC"
#       else
#           define PLATFORM_NAME "Linux" /* Generic Linux */
#       endif
#   endif

#endif

#if !defined(LINUX) && defined(LINUX_OS)
#   define LINUX 1
#endif


/* AIX */
#if defined(_AIX) && !defined(AIX_OS)
#   define AIX_OS
#endif


/* Android */
#if defined(__ANDROID__) && !defined(ANDROID_OS)
#   define ANDROID_OS
#endif

/**************************************************/

/* Der gesamte Code innerhalb dieses #ifndef-Blocks wird von Doxygen
 * für die Generierung der ERiC API HTML-Referenz ignoriert.
 */
#ifndef ERIC_DOXYGEN_MODE


typedef unsigned char BYTE;
typedef unsigned char byte;


#define ERIC_UINT32_MAX 0xffffffffUL

/**
 * @brief Definition eines vorzeichenlosen, 32 Bit breiten Integer-Typs.
 *
 * Der Typ uint32_t wird für Ganzzahl-Ausgaben der ERiC API verwendet, um
 * unabhängig von Betriebssystem und Compiler eine eindeutig spezifizierte
 * Bit-Breite und Kodierung zu gewährleisten.
 *
 * Die in ERiC implementierte Definition von uint32_t kommt nur zum Einsatz,
 * wenn im Code des ERiC-Anwenders kein Präprozessor-define gleichen Namens
 * existiert und wenn der Header <stdint.h> oder eine Compiler-spezifische
 * Variante davon nicht bereits importiert wurden.  Eventuelle Konflikte zu
 * gleichnamigen Typ-Definitionen im Code des ERiC-Anwenders können evtl. durch
 * Auskommentieren der ERiC-spezifischen Variante gelöst werden.
 *
 * @note In der HTML-Darstellung dieser Dokumentation wird keine konkrete
 * Implementierung angezeigt, sondern nur der Platzhalter
 * <i>__plattformabhaengige_Implementierung__</i>.  Die für eine bestimmte
 * Plattform tatsächlich verwendete Typdefinition wird aus dem betreffenden
 * Quell-Code in der Datei ericdef.h ersichtlich.
 */

#include <limits.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#if defined(_STDINT_H_) || defined(_STDINT_H) || defined(_STDINT) || defined(_MSC_STDINT_H_)
#    define ERIC_STDINT_VORHANDEN 1
#else
#    if defined(__has_include)
#        if __has_include("stdint.h")
#            include "stdint.h"
#            define ERIC_STDINT_VORHANDEN 1
#        endif
#    elif defined(__STDC__) && defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
         /* Wenn der Compiler den C99-Standard unterstützt, gibt es eine stdint.h */
#        include "stdint.h"
#        define ERIC_STDINT_VORHANDEN 1
#	elif (_MSC_VER >= 1600)
         /* Visual C++ bringt eine "stdint.h" ab Version 16 (d.h. VC++ 2010) mit */
#        include "stdint.h"
#        define ERIC_STDINT_VORHANDEN 1
#   endif
#endif

#if !defined(ERIC_STDINT_VORHANDEN)
#    define ERIC_STDINT_VORHANDEN 0
#    if !defined(uint32_t)
#        if (UINT_MAX == ERIC_UINT32_MAX)
             typedef unsigned int uint32_t;
#            define __uint32_t_defined
#        elif (USHRT_MAX == ERIC_UINT32_MAX)
             typedef unsigned short uint32_t;
#            define __uint32_t_defined
#        endif
#    endif
#endif

#else
/* Dieser Block wird von Doxygen für die Generierung der
   ERiC API HTML-Referenz ausgewertet */

/**
 * @brief Definition eines vorzeichenlosen, 32 Bit breiten Integer-Typs.
 *
 * Siehe Quellcode von platform.h für Implementierung.
 */
typedef __plattformabhaengige_Implementierung__ uint32_t;


#endif /* ERIC_DOXYGEN_MODE */


/*****************************************************/


#ifdef WINDOWS_OS
#  ifndef MSVC
#    define WINDOWS_GNUC true
#  endif
#endif


#ifdef WINDOWS_OS
#  define STDCALL               __stdcall
#  ifndef CDECL
#    define CDECL               __cdecl
#  endif

#else
#  if defined(__GNUC__) || defined(__GNUG__)
#    ifdef STDCALL
#      undef STDCALL
#    endif

#    ifdef CDECL
#      undef CDECL
#    endif

#    if defined(ARCH_CPU_X86) && defined(ARCH_BIT_32) && !defined(__IOS__) && !defined(ANDROID_OS)
#      define STDCALL           __attribute__((__stdcall__))
#    else
#      define STDCALL
#    endif
#    define CDECL               __attribute__((__cdecl__))
#  endif
#  if defined(_AIX)
#    ifdef STDCALL
#     undef STDCALL
#    endif
#    define STDCALL
#  endif
#endif

/************************************************/


#if _MSC_VER > 1000
#  ifndef MSVC
#    define MSVC
#  endif
#  pragma warning( disable : 4503 ) /* Laenge des ergaenzten Namens ueberschritten,                                                                         */
#  pragma warning( disable : 4786 ) /* Bezeichner wurde auf '255' Zeichen in den Browser-Informationen reduziert                                            */
#  pragma warning( disable : 4251 ) /* benoetigt eine DLL-Schnittstelle, die von Clients von class 'XYZ' verwendet wird                                     */
#  pragma warning( disable : 4355 ) /* this wird in initialisierungslisten verwendet                                                                        */
#  pragma warning( disable : 4275 ) /* class 'xxx' ist keine DLL-Schnittstelle und wurde als Basisklasse für die DLL-Schnittstelle class 'yyy' verwendet    */
#  pragma warning( disable : 4231 ) /* key word extern before template explicit instanciation is not standard but required for class EricSafeInt            */
#endif


/************************************************/


#ifdef MSVC
#  pragma warning( disable : 4250 ) /* 'class1' : inherits 'class2::member' via dominance */
#  define ATOI64 _atoi64
#  define I64(C)   C
#else
#  define ATOI64 atoll
#  define I64(C)   C##LL
#endif


#ifndef HAS_FUTIME
#  if defined(LINUX_OS) || defined(MAC_OS_X) || defined(__IOS__)
#    define HAS_FUTIME 0
#  else
#    define HAS_FUTIME 1
#  endif
#endif

#ifndef UTIME_NEEDS_CLOSED_FILE
#  if defined(LINUX_OS) || defined(MAC_OS_X) || defined(__IOS__)
#    define UTIME_NEEDS_CLOSED_FILE 1
#  else
#    define UTIME_NEEDS_CLOSED_FILE 0
#  endif
#endif

#if HAS_FUTIME && !defined(_AIX)
#  include <sys/utime.h>
#else
#  include <utime.h>
#endif

#endif /* ERIC_PLATFORM_H */
