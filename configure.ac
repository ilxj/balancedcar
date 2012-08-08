dnl Process this file with autoconf to produce a configure script.


AC_DEFUN([AX_CHECK_COMPILE_FLAG],
[AC_PREREQ(2.69)dnl for _AC_LANG_PREFIX
AS_VAR_PUSHDEF([CACHEVAR],[ax_cv_check_[]_AC_LANG_ABBREV[]flags_$4_$1])dnl
AC_CACHE_CHECK([whether _AC_LANG compiler accepts $1], CACHEVAR, [
  ax_check_save_flags=$[]_AC_LANG_PREFIX[]FLAGS
  _AC_LANG_PREFIX[]FLAGS="$[]_AC_LANG_PREFIX[]FLAGS $4 $1"
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM()],
    [AS_VAR_SET(CACHEVAR,[yes])],
    [AS_VAR_SET(CACHEVAR,[no])])
  _AC_LANG_PREFIX[]FLAGS=$ax_check_save_flags])
AS_IF([test x"AS_VAR_GET(CACHEVAR)" = xyes],
  [m4_default([$2], :)],
  [m4_default([$3], :)])
AS_VAR_POPDEF([CACHEVAR])dnl
])dnl AX_CHECK_COMPILE_FLAGS


AC_PREREQ(2.62)
AC_INIT(balance, 1.0)


AC_CANONICAL_SYSTEM
AM_INIT_AUTOMAKE([foreign])
AM_SILENT_RULES

AC_PROG_CC_C99
AC_PROG_RANLIB

platform=""

AC_ARG_WITH(platform,[AS_HELP_STRING([--with-platform=emu,avr],[default is emu])],[platform=$withval],[platform=emu])

AS_IF([ test "x$platform"="xemu"],
[ dnl platform is emulator
	AC_PROG_CXX
	AX_CHECK_COMPILE_FLAG([-std=gnu++11],[CXXFLAGS="$CXXFLAGS -std=gnu++11"],[
		dnl not support gnu++11? then c++11
			AX_CHECK_COMPILE_FLAG([-std=c++11],[CXXFLAGS="$CXXFLAGS -std=c++11"],
			 [ AC_MSG_FAILURE([please use c++11 compiler],[-1]) ])
	])
	
	dnl ----------------------
	AC_CHECK_LIB(glut,glutInit,)
	AC_CHECK_LIB(Box2D,main,)
	AC_CHECK_LIB(rt,clock_gettime,)
])

AM_CONDITIONAL(platformemu,[ test "x$platform"="xemu"])


AC_CONFIG_FILES(Makefile algorithm/Makefile platform/Makefile)
AC_OUTPUT

