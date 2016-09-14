AC_PREREQ([2.68])
AC_INIT(achart, 0.1, [lpmichalik@googlemail.com])

AM_INIT_AUTOMAKE

AC_PROG_CXX

AC_CHECK_PROG(HAVE_ASCIIDOC, [asciidoc], [yes], [no])
AC_CHECK_PROG(HAVE_XMLTO, [xmlto], [yes], [no])
if test "x$HAVE_ASCIIDOC" = xno -o "x$HAVE_XMLTO" = xno; then
    AC_MSG_WARN([]
    [**********]
    [********** asciidoc or xmlto not found.  The manpage will not be build.  It can still be viewed in asciidoc format in the doc/ directory.]
    [**********])
fi
AM_CONDITIONAL(ACHART_MANPAGE, [test "x$HAVE_ASCIIDOC" = xyes -a "x$HAVE_XMLTO" = xyes])

AC_CHECK_LIB([nova], [ln_deg_to_rad], , [AC_MSG_ERROR([Required library libnova not found!])])

ACHARTS_CXXFLAGS="-Wall -Wextra -pedantic -std=c++11"
AC_SUBST(ACHARTS_CXXFLAGS)

AC_LANG(C++)

AC_MSG_CHECKING([for boost libraries])
AC_COMPILE_IFELSE(
     [AC_LANG_PROGRAM([[
     #include <boost/algorithm/string/trim.hpp>
     #include <boost/spirit/include/qi.hpp>
     ]],[[
     boost::spirit::qi::rule<const char*, std::string> r;
     ]])],
[AC_MSG_RESULT([yes])],
[AC_MSG_ERROR([[
Boost libraries not found.  acharts needs header only boost libraries: algorithm and spririt.  Some distributions provide those in libboost-dev package.]])])

AC_OUTPUT([Makefile doc/Makefile src/Makefile examples/Makefile])
