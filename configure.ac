AC_INIT(gnome-globalmenu, [0.6.9], [rainwoodman@gmail.com], [gnome-globalmenu])

AM_INIT_AUTOMAKE([1.9 tar-ustar])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS(config.h)

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_PATH_PROG(VALAC, valac, valac)
AC_SUBST(VALAC)

AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)

GLIB_REQUIRED=2.10.0

PKG_CHECK_MODULES(GLIB, glib-2.0 >= $GLIB_REQUIRED gobject-2.0 >= $GLIB_REQUIRED)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

AC_CONFIG_FILES([Makefile
			gee/Makefile
           gee/gee/Makefile
		   gee/dom-gee-1.0.pc
           gee/tests/Makefile
		   dom/Makefile
		   dom/dom-1.0.pc
		   dom/dom/Makefile
		   dom/tests/Makefile
		   ])

AC_OUTPUT
