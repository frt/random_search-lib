AC_PREREQ([2.65])
AC_INIT([random_search-lib], [0.1], [fr.teodoro@gmail.com])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])

m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB

# Checks for header files.
AC_CHECK_HEADER([stdlib.h], [], [AC_MSG_ERROR(stdlib.h missing)])
AC_CHECK_HEADER([mtwist.h], [], [AC_MSG_ERROR(mtwist.h missing (get it at https://github.com/frt/mtwist))])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile src/Makefile])

AC_OUTPUT
