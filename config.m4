PHP_CHECK_LIBRARY(
	ericapi,
	EricInitialisiere,
	[
		PHP_ADD_LIBRARY_WITH_PATH(ericapi, ./lib, EXTRA_CFLAGS)
		AC_DEFINE(HAVE_ERICAPILIB, 1, [ ])
	],[
		AC_MSG_ERROR([lib error])	
	],
	[
		-L./lib -lm
	]
)

PHP_SUBST(EXTRA_CFLAGS)

PHP_NEW_EXTENSION(eric, php_eric.c, $ext_shared)
