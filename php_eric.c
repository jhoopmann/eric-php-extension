

#include "php.h"

#include "php_eric.h"
#include "include/ericapi.h"
#include "include/eric_fehlercodes.h"

PHP_FUNCTION(eric_init);
ZEND_BEGIN_ARG_INFO_EX(arginfo_eric_init, 0, 0, 2)
    ZEND_ARG_INFO(0, plugin_path)
    ZEND_ARG_INFO(0, log_path)
ZEND_END_ARG_INFO()

PHP_FUNCTION(eric_close);


static zend_function_entry eric_functions[] = {
	PHP_FE(eric_init, arginfo_eric_init)
    PHP_FE(eric_close, NULL)
    PHP_FE_END
};

zend_module_entry eric_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_ERIC_EXTNAME,
    eric_functions,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
#if ZEND_MODULE_API_NO >= 20010901
    PHP_ERIC_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

ZEND_GET_MODULE(eric);

PHP_FUNCTION(eric_init)
{
    const char* plugin_path;
    int plen;
    const char* log_path;
    int llen;

    ZEND_PARSE_PARAMETERS_START(2,2)
        Z_PARAM_STRING(plugin_path, plen);
        Z_PARAM_STRING(log_path, llen);
    ZEND_PARSE_PARAMETERS_END();

    if(EricInitialisiere(plugin_path, log_path) == ERIC_OK)  {
        RETURN_BOOL(IS_TRUE);
    }
}

PHP_FUNCTION(eric_close)
{
    EricBeende();
}