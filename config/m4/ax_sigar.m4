##*****************************************************************************
## $Id$
##*****************************************************************************
#  SYNOPSIS:
#    AC_SIGAR
#
#  DESCRIPTION:
#    Checks for sigar
##*****************************************************************************

AC_DEFUN([AC_SIGAR],
[
  #
  # Check for SIGAR
  # 
  AC_ARG_WITH([sigar], 
      AC_HELP_STRING([--with-sigar=PATH],
      [set PATH to the installation of sigar library [[default=/usr]]]),
      [SIGAR_LIB=${with_sigar}],
      [SIGAR_LIB="/usr"])

  AC_CHECK_HEADER([${SIGAR_LIB}/sigar-bin/include/sigar.h], [], [
      AC_MSG_ERROR([Could not find the 'sigar.h' header.
  Use the '--with-sigar=PATH' option to specify location of SIGAR])])

  CXXFLAGS="${CXXFLAGS# } -I${with_sigar}/sigar-bin/include"
  LDFLAGS="${LDFLAGS# } -L${with_sigar}/sigar-bin/lib"


  #
  # Check for whether to build sigar module
  # 
  AC_MSG_CHECKING([for os type ($host_os)])
  case "$host_os" in
     *darwin*)
     SRC_OS="darwin"
     AC_DEFINE(DARWIN,                [], [running on MacOS X]) 
     AC_DEFINE(SIGAR_TEST_OS_DARWIN, [1], [for the tests])
     SIGAR_LIB_NAME="-lsigar-universal64-macosx"
     SIGAR_LIB_FULLNAME="libsigar-universal64-macosx.so"
     ;;  
     *linux*)
     SRC_OS="linux"
     AC_DEFINE(SIGAR_TEST_OS_LINUX,  [1], [for the tests])
     SIGAR_LIB_NAME="-lsigar-amd64-linux"
     SIGAR_LIB_FULLNAME="libsigar-amd64-linux.so"
     ;;  
     *)  
     ac_system="unknown"
  esac
  AC_MSG_RESULT([$SRC_OS])
  AC_SUBST(SRC_OS)
  AM_CONDITIONAL(OS_MACOSX, test x$SRC_OS = xdarwin)
  AM_CONDITIONAL(OS_LINUX,  test x$SRC_OS = xlinux)

  AC_CHECK_HEADERS(utmp.h utmpx.h libproc.h valgrind/valgrind.h)
  if test $ac_cv_header_libproc_h = yes; then
          AC_DEFINE(DARWIN_HAS_LIBPROC_H, [1], [sigar named them DARWIN_HAS_... instead of HAVE_]) 
  fi

  AC_SUBST(SIGAR_LIB)
  AC_SUBST(SIGAR_LIB_NAME)
  AC_SUBST(SIGAR_LIB_FULLNAME)
  AC_SUBST(HAVE_SIGAR)
])
