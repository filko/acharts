AC_PREREQ([2.68])
AC_INIT(achart, 0.1, [lmi@ift.uni.wroc.pl])

AM_INIT_AUTOMAKE

AC_PROG_CXX

AC_CHECK_PROG(HAVE_ASCIIDOC, [asciidoc], [yes], [no])
if test "x$HAVE_ASCIIDOC" = xno ; then
    AC_MSG_ERROR([asciidoc not found])
fi

AC_CHECK_PROG(HAVE_XMLTO, [xmlto], [yes], [no])
if test "x$HAVE_XMLTO" = xno ; then
    AC_MSG_ERROR([xmlto not found])
fi

AC_CHECK_LIB([nova], [ln_deg_to_rad], , [AC_MSG_ERROR([Required library libnova not found!])])

ACHARTS_CXXFLAGS="-Wall -Wextra -pedantic -std=c++0x"
AC_SUBST(ACHARTS_CXXFLAGS)

AC_OUTPUT([Makefile doc/Makefile src/Makefile examples/Makefile])
