PHP_ARG_ENABLE(eric,
    [Whether to enable the "ERiC" extension],
    [  --enable-eric         Enable "ERiC" extension support])

if test $PHP_ERIC != "no"; then
    PHP_REQUIRE_CXX()
    PHP_SUBST(ERIC_SHARED_LIBADD)
    PHP_NEW_EXTENSION(eric, php_eric.c, $ext_shared)
fi