AC_INIT([fxite],0.93, [yetanothergeek@gmail.com])

PACKAGE="fxite"
VERSION=0.93

AC_SUBST([VERSION])
AC_SUBST([PACKAGE])

AM_INIT_AUTOMAKE([-Wall])

AM_GNU_GETTEXT([external])

AC_PROG_CC
AC_PROG_CXX
AC_LANG_C
AC_PROG_MAKE_SET
AC_PROG_RANLIB
AC_HEADER_STDC

dnl #Let pkgconfig handle FOX flags unless they are set in the environment
if test "x${FOX_LIBS}${FOX_CFLAGS}" = "x"
then
  PKG_PROG_PKG_CONFIG()
  FOX_PKG_NAME=fox
  for L in fox18 fox17 fox
  do
    AC_MSG_CHECKING([for $L.pc])
    if ${PKG_CONFIG} --exists ${L}
    then
      AC_MSG_RESULT([yes])
      FOX_PKG_NAME=${L}
      break
    else
      AC_MSG_RESULT([no])
    fi
  done
  PKG_CHECK_MODULES([FOX], [${FOX_PKG_NAME} >= 1.7],
    [
      AC_MSG_RESULT([Found FOX-1.7 or newer])
      AC_DEFINE(FOX_1_7)
    ],
    [
      AC_MSG_RESULT([Could not find FOX-1.7 or newer])
      PKG_CHECK_MODULES([FOX], [fox >= 1.6],
        AC_MSG_RESULT([Found FOX-1.6.x])
        AC_DEFINE(FOX_1_6))
    ]
  )
else
  if test "x${FOX_LIBS}" = "x"
  then
    AC_MSG_ERROR([If you set FOX_CFLAGS, you must also set FOX_LIBS])
  fi
  if test "x${FOX_CFLAGS}" = "x"
  then
    AC_MSG_ERROR([If you set FOX_LIBS, you must also set FOX_CFLAGS])
  fi
  AC_MSG_CHECKING([for FOX version in FOX_LIBS])
  case "${FOX_LIBS}" in
    *FOX-1.6*)
      AC_MSG_RESULT([looks like version 1.6])
      AC_DEFINE(FOX_1_6)
    ;;
    *FOX-1.7*|*FOX-1.8*)
      AC_MSG_RESULT([looks like version 1.7 or newer])
      AC_DEFINE(FOX_1_7)
    ;;
    *)
      AC_MSG_RESULT([FAILED])
      AC_MSG_WARN([could not find FOX version, assuming 1.6])
      AC_DEFINE(FOX_1_6)
    ;;
  esac
fi


AC_ARG_WITH([system-lua],
  AC_HELP_STRING([--with-system-lua],[use installed Lua library]),
  [
    AC_ARG_WITH([lua-pkg],
     AC_HELP_STRING([--with-lua-pkg=ARG],[name of Lua pkg-config script [[default=lua5.1]]]),
     if test "x${withval}" = "xyes"
     then
       AC_MSG_ERROR([--with-lua-pkg requires an argument.])
     fi
     [LUA_PKG_NAME=${withval%.pc}],
    [
    LUA_PKG_NAME=lua5.1
    for L in lua5.1 lua51 lua-5.1 lua
    do
      AC_MSG_CHECKING([for $L.pc])
      if ${PKG_CONFIG} --exists ${L}
      then
        AC_MSG_RESULT([yes])
        LUA_PKG_NAME=${L}
        break
      else
        AC_MSG_RESULT([no])
      fi
    done
    ]
    )
    PKG_CHECK_MODULES([LUA], [${LUA_PKG_NAME} >= 5.1.2])
    AM_CONDITIONAL(WITH_SYSTEM_LUA, true)
  ]
  ,
  [
    dnl Use local copy of Lua
    LUA_CFLAGS="-I\${abs_top_srcdir}/lua"
    LUA_LIBS="-Wl,-E -L\${abs_top_builddir}/lua -llua -lm"
    AM_CONDITIONAL(WITH_SYSTEM_LUA, false)
  ]
)



AC_CANONICAL_HOST
case $host_os in
  *linux* )
    dnl #Tell Lua sources if we are on Linux
    AC_DEFINE(LUA_USE_LINUX)
    dnl #Fedora-13 needs explicit libdl, should be harmless otherwise
    LUA_LIBS="${LUA_LIBS} -ldl"
    PKG_CHECK_MODULES([X11],[x11])
  ;;
  *mingw*)
    dnl #tell mingw where to find stuff
    FOX_LIBS="\${abs_top_srcdir}/src/icon32x32.coff ${FOX_LIBS} -lgdi32 -limm32 -lws2_32 -lole32 -luuid"
    AC_ARG_ENABLE([win32-console],AC_HELP_STRING([--enable-win32-console],[enable win32 debug console]))
    AC_MSG_RESULT([$enable_win32_console])
    if test "x$enable_win32_console" != "xyes"
    then
      FOX_LIBS="${FOX_LIBS} -mwindows"
    fi
  ;;
  *minix*)
    AC_DEFINE(_MINIX)
    AC_DEFINE(_POSIX_SOURCE)
    AC_DEFINE(LUA_USE_POSIX)
    PKG_CHECK_MODULES([X11],[x11])
    FOX_LIBS="${FOX_LIBS} -lpthread"
  ;;
  *freebsd*)
    AC_DEFINE(LUA_USE_POSIX)
    PKG_CHECK_MODULES([X11],[x11])
  ;;
esac



dnl #Make sure all Lua functions get linked in.
case "${LUA_LIBS}" in
  *-Wl,-E*) ;;
  *) LUA_LIBS="-Wl,-E ${LUA_LIBS}"
esac


FXSCINTILLA_CFLAGS="-I\${abs_top_srcdir}/fxscintilla"
FXSCINTILLA_LIBS="-L\${abs_top_builddir}/fxscintilla -lfxscintilla"

JEFFX_LIBS="-L\${abs_top_builddir}/jef -ljeffx"
JEFFX_CFLAGS="-I\${abs_top_srcdir}/jef"

MNUMGR_LIBS="-L\${abs_top_builddir}/mnumgr -lmnumgr"
MNUMGR_CFLAGS="-I\${abs_top_srcdir}/mnumgr"



AC_ARG_ENABLE([assert],AC_HELP_STRING([--enable-assert],[enable assertions]))

AC_MSG_RESULT([$enable_assert])

if test "x$enable_assert" = "xyes"
then
  CFLAGS="${CFLAGS} -DDEBUG"
  CXXFLAGS="${CXXFLAGS} -DDEBUG"
else
  CFLAGS="${CFLAGS} -DNDEBUG"
  CXXFLAGS="${CXXFLAGS} -DNDEBUG"
fi


if test "x${RESWRAP}" = "x"
then
  AC_PATH_PROGS(RESWRAP, reswrap-1.7 reswrap)
fi


AC_SUBST(FOX_CFLAGS)
AC_SUBST(FOX_LIBS)

AC_SUBST(LUA_CFLAGS)
AC_SUBST(LUA_LIBS)

AC_SUBST(FXSCINTILLA_CFLAGS)
AC_SUBST(FXSCINTILLA_LIBS)

AC_SUBST(X11_CFLAGS)
AC_SUBST(X11_LIBS)

AC_SUBST(JEFFX_CFLAGS)
AC_SUBST(JEFFX_LIBS)

AC_SUBST(MNUMGR_CFLAGS)
AC_SUBST(MNUMGR_LIBS)


AC_OUTPUT( 
 Makefile
 fxscintilla/Makefile
 lua/Makefile 
 src/Makefile
 src/languages/Makefile
 po/Makefile.in
 mnumgr/Makefile
 jef/Makefile
)

