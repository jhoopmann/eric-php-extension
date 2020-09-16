

#include "php.h"

#include "php_eric.h"
#include "include/ericapi.h"

PHP_FUNCTION(eric_init);

static zend_function_entry eric_functions[] = {
	PHP_FE(eric_init, NULL)
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
    
}