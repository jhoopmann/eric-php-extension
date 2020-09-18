#ifndef PHP_ERIC_H
#define PHP_ERIC_H
#endif

#define PHP_ERIC_VERSION "1.0"
#define PHP_ERIC_EXTNAME "eric"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern zend_module_entry eric_module_entry;
#define eric_module_ptr &eric_module_entry
#define phpext_eric_ptr eric_module_ptr

#ifdef PHP_WIN32
#define PHP_ERIC_API __declspec(dllexport)
#else
#define PHP_ERIC_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

void *lericapi;

int* (*pEricInitialisiere)(const char*, const char*);
void* (*pEricBeende)();