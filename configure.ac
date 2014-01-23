AC_INIT(yaplot.c)
AC_CONFIG_HEADERS(config.h)
AM_INIT_AUTOMAKE(yaplot,3.5)

dnl Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_INSTALL(install-sh)

AC_PATH_X
AC_PATH_XTRA
dnl AC_CHECK_LIB(X11, XOpenDisplay, x_libs="-lX11 $X_EXTRA_LIBS", 
dnl   AC_MSG_ERROR([*** libX11 not found. Check 'config.log' for more details.]),
dnl   $X_EXTRA_LIBS)

dnl Replace `main' with a function in -lm:
AC_CHECK_LIB(m, main)
AC_SUBST(HAVE_POWIF)
AC_CHECK_LIB(m, powif,CPPFLAGS="$CPPFLAGS -DHAVE_POWIF")

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(sys/time.h unistd.h)
AC_CHECK_HEADERS(gtk/gtk.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T


AC_SUBST(X_LIBS)
AC_SUBST(X_CFLAGS)

dnl AC_ARG_PROGRAM

dnl Test for pkg-config
PKG_PROG_PKG_CONFIG([0.22])

AC_SUBST(YAPLOT)
dnl AC_ARG_WITH(gtk,[  --with-gtk              use gtk to compile yaplot])
dnl AC_ARG_WITH(glut,[  --with-glut              use glut to compile yaplot])
YAPLOT=yaplot
dnl if test "$with_gtk" != "no" ; then
  dnl YAPLOT="$YAPLOT yaplot_gtk"
  dnl AM_PATH_GTK(1.2.0,,AC_MSG_ERROR($PACKAGE $VERSION needs gtk))
  dnl AM_PATH_GTK_2_0(2.0.0,, [AC_MSG_ERROR(Test for GTK failed.)])
  PKG_CHECK_MODULES(GTK, [gtk+-2.0 >= 2.12])
  
  X_LIBS=$GTK_LIBS
  X_CFLAGS=$GTK_CFLAGS
  dnl AC_PATH_PROG(GDK_PIXBUF_CONFIG, gdk-pixbuf-config, no)
  AC_PATH_PROG(GDK_PIXBUF_CSOURCE,[gdk-pixbuf-csource],no)
  AC_PATH_PROG(PKG_CONFIG,[pkg-config],no)

  dnl Replace `main' with a function in -lgdk:
  AC_CHECK_LIB(gdk, main)
  dnl Replace `main' with a function in -lglib:
  AC_CHECK_LIB(glib, main)
  dnl Replace `main' with a function in -lgmodule:
  AC_CHECK_LIB(gmodule, main)
  dnl Replace `main' with a function in -lgtk:
  AC_CHECK_LIB(gtk, main)
  CPPFLAGS="$CPPFLAGS -DYAPLOT_GTK"
  if test "$PKG_CONFIG" != "no" ; then
    if pkg-config gdk-pixbuf-2.0 ; then
      dnl AC_CHECK_LIB(png, main)
      AC_CHECK_LIB(jpeg, main)
      AC_MSG_WARN([gdk-pixbuf is found, recording feature is enabled.])
      LIBS="$LIBS          `$PKG_CONFIG --libs gdk-pixbuf-2.0`"
      LIBS="$LIBS          `$PKG_CONFIG --libs libpng`"
      CPPFLAGS="$CPPFLAGS -DRECORD `$PKG_CONFIG --cflags gdk-pixbuf-2.0`"
      AC_CHECK_HEADERS(gdk-pixbuf/gdk-pixbuf.h)
    else
      AC_MSG_WARN([Could not find gdk-pixbuf, recording feature is disabled.])
    fi
  else
    AC_MSG_WARN([Could not find pkg-config....])
  fi
dnl fi
dnl if test "$with_glut" != "no" ; then
dnl   AC_MSG_WARN([Yaplot will be compiled with glut.])
dnl  YAPLOT="$YAPLOT yaplot_glut"
dnl  X_LIBS="$X_LIBS -lglut -lGLU -lGL"
PKG_CONFIG_PATH="$PKG_CONFIG_PATH":teststring
dnl  dnl for OpenGL/glut
dnl  AC_CHECK_LIB(GL, main)
dnl  AC_CHECK_LIB(GLU, main)
dnl  AC_CHECK_LIB(glut, main)
dnl  AC_CHECK_LIB(Xmu, main)
dnl  AC_CHECK_LIB(Xi, main)
dnl  AC_CHECK_LIB(Xext, main)
dnl  dnl opengl done
dnl fi


#GNOME_COMMON_INIT
#GNOME_INIT
#GNOME_COMPILE_WARNINGS

AC_OUTPUT(Makefile)
