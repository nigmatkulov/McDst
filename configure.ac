AC_INIT([McDst], [1.0], [])

dnl Library names (subst down below)
dnl 'lib' prefix will be added later as we need to use these
dnl variables with the linker option -l
MCDST=McDst

dnl Debug option
AC_ARG_ENABLE(debug,
              AS_HELP_STRING([--enable-debug],[enable debug information without optimizations (-O0), defult: no]),
              [case "${enableval}" in
               yes) debug=true  ;;
               no)  debug=false ;;
               *)   AC_MSG_ERROR([bad value ${enablevalue} for --enable-opt-debug]) ;;
           esac], [debug=false])

dnl Optimized debug option
AC_ARG_ENABLE(optdebug,
              AS_HELP_STRING([--enable-opt-debug],[enable debug information with optimizations (-O2), defult: no]),
              [case "${enableval}" in
               yes) optdebug=true  ;;
               no)  optdebug=false ;;
               *)   AC_MSG_ERROR([bad value ${enablevalue} for --enable-opt-debug]) ;;
           esac], [optdebug=false])

dnl Converter options
AC_ARG_ENABLE(urqmd,
              AS_HELP_STRING([--enable-urqmd],[enable UrQMD converter, defult: yes]),
              [case "${enableval}" in
               yes) urqmd=true  ;;
               no)  urqmd=false ;;
               *)   AC_MSG_ERROR([bad value ${enablevalue} for --enable-urqmd]) ;;
           esac], [urqmd=true])

AC_ARG_ENABLE(pythia8,
              AS_HELP_STRING([--enable-pythia8],[enable Pythia 8 converter, defult: no]),
              [case "${enableval}" in
               yes) pythia8=true  ;;
               no)  pythia8=false ;;
               *)   AC_MSG_ERROR([bad value ${enablevalue} for --enable-pythia8]) ;;
           esac], [pythia8=false])
AC_ARG_WITH(pythia8, AS_HELP_STRING([--with-pythia8=ARG], [path to the directory with pythia8-config, default: using $PATH]),
[pythia8_root=; [ test "x${withval-}" == "xno"] || pythia8_root="${withval-}"], [pythia8_root=])

dnl Determine the OS
AC_CHECK_PROG([UNAME], [uname], [uname])
[ test -n "$UNAME" ] || AC_MSG_ERROR(['uname' not found])
AC_MSG_CHECKING([for OS])
case "$(uname -s)" in
    Linux*)
        SUFFIX=so
        LDFLAG_DSO="-shared"
        AC_MSG_RESULT([Linux])
        ;;
    Darwin*)
        SUFFIX=dylib
        LDFLAG_DSO="-dynamiclib"
        AC_MSG_RESULT([Darwin])
        ;;
    *)
        AC_MSG_ERROR([OS ${host_os} is not supported])
        ;;
esac

if [ "$pythia8" == "true" ]; then
    if [ test -n "$pythia8_root" ]; then
        AC_CHECK_PROG([PYTHIA8], [pythia8-config], [$pythia8_root/pythia8-config], [], [path = $pythia8_root])
   else
       AC_CHECK_PROG([PYTHIA8], [pythia8-config], [pythia8-config])
   fi
   [ test -n "$PYTHIA8" ] || AC_MSG_ERROR([pythia8-config not found])
   CONVERTERS="pythia8"
   AC_MSG_NOTICE([pythia8 converter is on])
fi

if [ "$urqmd" == "false" ]; then
    CONVERTERS="urqmd2mc $CONVERTERS"
    AC_MSG_NOTICE([urqmd converter is on])
fi

dnl TODO: add dependences checking

dnl Set default CXXFLAGS in the case if it is _not_ set yet
if [ test -z "${CXXFLAGS-}" ]; then
    if [ test "x$debug" == "xtrue" ]; then
        CXXFLAGS="-g -O0"
    else
        CXXFLAGS="-O2"
    fi
    dnl Define flags. -D_VANILLA_ROOT_ is needed to avoid StMessMgr confusion
    CXXFLAGS="$CXXFLAGS $(root-config --cflags) -fPIC -W -Woverloaded-virtual -Wno-deprecated-declarations -Wall -pipe -I. -I.. -std=c++11 -D__ROOT__"
fi

dnl Set LDFLAGS if it is _not_ set yet
if [ test -z "${LDFLAGS-}" ]; then
    LDFLAGS="$(root-config --glibs) -lEG"
fi

AC_PROG_CXX

dnl Configure Makefile.in
AC_CONFIG_FILES([Makefile.auto])

dnl Substitutions for Makefiles
AC_SUBST(LDFLAG_DSO, "$LDFLAG_DSO")
AC_SUBST(MCDST, "$MCDST")
AC_SUBST(SUFFIX, "$SUFFIX")
[ "$pythia8" == "true" ] || AC_SUBST(PYTHIA8, "$PYTHIA8")
AC_SUBST(CONVERTERS, "$CONVERTERS")

AC_OUTPUT
