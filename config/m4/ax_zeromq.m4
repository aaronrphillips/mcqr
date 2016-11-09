##*****************************************************************************
## $Id$
##*****************************************************************************
#  SYNOPSIS:
#    AC_ZEROMQ
#
#  DESCRIPTION:
#    Checks for whether to include sigar module
##*****************************************************************************

AC_DEFUN([AC_ZEROMQ],
[
  #
  # Check for ZeroMQ
  # 
  AC_ARG_WITH([zeromq], 
      AC_HELP_STRING([--with-zeromq=PATH],
      [set PATH to the installation of zeromq library [[default=/usr]]]),
      [ZEROMQ_LIB=${with_zeromq}],
      [ZEROMQ_LIB="/usr"])


  AC_CHECK_HEADER([${ZEROMQ_LIB}/include/zmq.h], [], [
      AC_MSG_ERROR([Could not find the 'zmq.h' header.
  Use the '--with-zeromq=PATH' option to specify location of ZeroMQ])])

  CXXFLAGS="${CXXFLAGS# } -I${with_zeromq}/include"

  #
  # Check for cppzmq
  #
  AC_ARG_WITH([cppzmq], 
      AC_HELP_STRING([--with-cppzmq=PATH],
      [set PATH to the installation of cppzmq headers. See https://github.com/zeromq/cppzmq [[default=/usr]]]),
      [CPPZMQ_LIB=${with_cppzmq}],
      [CPPZMQ_LIB="/usr"])

  AC_LANG_PUSH([C++])
  AC_CHECK_HEADER([${CPPZMQ_LIB}/zmq.hpp], [], [
      AC_MSG_ERROR([Could not find the 'zmq.hpp' header.
  Use the '--with-cppzmq=PATH' option to specify location of cppzmq. For source see https://github.com/zeromq/cppzmq])])
  AC_LANG_POP([C++])


  CXXFLAGS="${CXXFLAGS# } -I${with_cppzmq}"

  case "${enable_static}" in
      *yes*)
      LDFLAGS="${LDFLAGS# } ${with_zeromq}/lib/libzmq.a"
      ;;  
      *)  
      case "${host_os}" in
          *darwin*)
              LDFLAGS="${LDFLAGS# } -L${with_zeromq}/lib -lzmq"
          ;;  
          *)  
              LDFLAGS="${LDFLAGS# } -L${with_zeromq}/lib -lzmq"
          ;;  
      esac
      ;;  
  esac

  AC_MSG_NOTICE([ZeroMQ:  ${ZEROMQ_LIB}])
  AC_SUBST(ZEROMQ_LIB)
  AC_SUBST(HAVE_ZEROMQ)
])
