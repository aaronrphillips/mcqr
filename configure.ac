AC_INIT([mcqr], [1_0_2], [aaron.phillips@pnnl.gov])
AC_PREREQ([2.59])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_SRCDIR([src/mcqueuer.cpp])
AC_CONFIG_HEADERS([src/config.h:src/config.h.in])
AC_CANONICAL_SYSTEM
AM_MAINTAINER_MODE

AM_CONDITIONAL(MAINTAINER, test "$enable_maintainer_mode" = yes)


AC_LANG([C++])
AC_PROG_CXX([g++])
AC_PATH_PROG([CXX], [g++], [no])
if test x"${CXX}" == x"no" ; then
  AC_MSG_ERROR(Could not find GNU g++ in your path!)
fi
AX_CXX_COMPILE_STDCXX_11(ext,mandatory)


dnl  Checks for libraries.

AC_CHECK_LIB([pthread_nonshared], [m])

dnl  Checks for library functions.

AC_FUNC_FORK


dnl   Checks for header files.

AC_CHECK_HEADERS([algorithm climits cmath cstdio errno.h execinfo.h fstream grp.h iomanip iostream libgen.h limits list map math.h ostream process.h signal.h sstream stdlib.h string syslog.h time.h unistd.h vector windows.h])
dnl #include <algorithm>
dnl #include <climits>
dnl #include <cmath>
dnl #include <fstream>
dnl #include <iomanip>
dnl #include <iostream>
dnl #include <limits>
dnl #include <list>
dnl #include <map>
dnl #include <ostream>
dnl #include <sstream>
dnl #include <string>
dnl #include <vector>
dnl 
dnl #include <arpa/inet.h>
dnl #include <cxxabi.h>
dnl #include <errno.h>
dnl #include <execinfo.h>
dnl #include <grp.h>
dnl #include <linux/if_packet.h>
dnl #include <linux/types.h>
dnl #include <math.h>
dnl #include <net/ethernet.h>
dnl #include <net/if.h>
dnl #include <process.h>
dnl #include <signal.h> 
dnl #include <stdio.h>
dnl #include <stdlib.h>
dnl #include <sys/ioctl.h>
dnl #include <syslog.h>
dnl #include <sys/socket.h>
dnl #include <sys/types.h>
dnl #include <time.h> 
dnl #include <unistd.h>
dnl #include <windows.h>


dnl  Checks for typedefs, structures, and compiler characteristics.

AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT32_T


AM_INIT_AUTOMAKE([foreign subdir-objects tar-ustar])


m4_include([config/m4/ax_sigar.m4])
m4_include([config/m4/ax_zeromq.m4])


AC_SIGAR
AC_ZEROMQ

#AC_CONFIG_FILES([Makefile src/Makefile test/Makefile])
AC_CONFIG_FILES([Makefile src/Makefile ])
AC_OUTPUT
