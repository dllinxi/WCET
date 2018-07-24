AC_INIT(glue.cpp)
AM_INIT_AUTOMAKE(cyacas, 0.1)
AM_CONFIG_HEADER(glue.h)

AC_PROG_CC
AC_PROG_CXX
AC_LANG(C++)
AC_PROG_RANLIB
AC_CHECK_PROG(OCAMLC,ocamlc,ocamlc,no)
if test "$OCAMLC" = no ; then
	AC_MSG_ERROR(Cannot find ocamlc.)
fi
AC_CHECK_PROG(OCAMLMKTOP,ocamlmktop,ocamlmktop,no)
if test "$OCAMLMKTOP" = no ; then
	AC_MSG_ERROR(Cannot find ocamlmktop.)
fi
AM_PROG_LD
AC_ARG_WITH(yacas, [  --with-yacas=...     Specify the location of YACAS],,[ with_yacas="/usr" ])
CPPFLAGS="-I${with_yacas}/share/yacas/include/ -I${with_yacas}/share/yacas/include/plat/linux32/"
AC_CHECK_HEADER( [yacas.h],, AC_MSG_ERROR(Cannot find yacas header files),[])
AC_DEFINE_UNQUOTED(YACASPATH, "${with_yacas}", [Yacas installation path])
AC_SUBST(camldir, "${libdir}")
AC_SUBST(yacas_libdir, "${with_yacas}/lib")
AC_SUBST(PREFIX, ${prefix})
AC_OUTPUT([Makefile yacastop])
