#include "php_eric.h"

PHP_MINIT_FUNCTION(eric)
{
    lericapi = dlopen("/home/jhoopmann/projects/eric-php-extension/lib/libericapi.so", RTLD_NOW);
    if(!lericapi) {
        printf("cant load lericapi\n");
        exit(1);
    }

    pEricInitialisiere = dlsym(lericapi, "EricInitialisiere");
    pEricBeende = dlsym(lericapi, "EricBeende");
}

PHP_MSHUTDOWN_FUNCTION(eric)
{
    if(lericapi) {
        dlclose(lericapi);
    }
}

PHP_FUNCTION(eric_init)
{
    if(pEricInitialisiere(
            getenv("ERICAPI_LIB_PATH"), 
            getenv("ERICAPI_LOG_PATH")
        ) == ERIC_OK
    )  {
        RETURN_BOOL(IS_TRUE);
    }
}

PHP_FUNCTION(eric_close)
{
    if(lericapi) {
        pEricBeende();
    }
}

static zend_function_entry eric_functions[] = {
	PHP_FE(eric_init, NULL)
    PHP_FE(eric_close, NULL)
    PHP_FE_END
};

zend_module_entry eric_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_ERIC_EXTNAME,
    eric_functions,
    PHP_MINIT(eric),
    PHP_MSHUTDOWN(eric),
    NULL,
    NULL,
    NULL,
#if ZEND_MODULE_API_NO >= 20010901
    PHP_ERIC_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(eric);

