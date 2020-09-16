/*
 * Copyright © LfSt Bayern, Muenchen, IuK 111, 2006
 */

#ifndef ERIC_API_EXPORT_H_
#define ERIC_API_EXPORT_H_

/**
 * @file
 * @brief Attribute für dynamische Bibliotheken
 *
 * Diese Deklarationen sind für Windows-Plattformen relevant.
 */

#include "platform.h"

#if defined(WINDOWS_OS) && !defined(ERICAPI_DLL)
#define ERICAPI_IMPORT __declspec(dllimport)
#else
#define ERICAPI_IMPORT
#endif

#endif /* ERIC_API_EXPORT_H_ */
