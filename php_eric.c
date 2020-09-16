

#include "php.h"

#include "php_eric.h"


PHP_FUNCTION(test);

static zend_function_entry eric_functions[] = {
	PHP_FE(test)
	{NULL,NULL,NULL}
};

zend_module_entry eric_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    PHP_ERIC,
    hello_functions,
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

PHP_FUNCTION(test)
{
    php_printf("test\n");
}