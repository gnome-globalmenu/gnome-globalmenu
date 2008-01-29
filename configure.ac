AC_INIT(gnome-globalmenu, 0.4)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
GNOME_COMMON_INIT
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE
GNOME_MAINTAINER_MODE_DEFINES

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_PROG_LIBTOOL

PKG_CHECK_MODULES(LIBGNOMENU, [gtk+-2.0 >= 2.10 gdk-2.0])
PKG_CHECK_MODULES(GNOME_GLOBALMENU_APPLET, [gtk+-2.0 >= 2.10, gdk-2.0])
PKG_CHECK_MODULES(TEST_SUITE, [gtk+-2.0 >= 2.10, gdk-2.0])



AC_SUBST(LIBGNOMENU_CFLAGS)
AC_SUBST(LIBGNOMENU_LIBS)

AC_SUBST(GNOME_GLOBALMENU_APPLET_CFLAGS)
AC_SUBST(GNOME_GLOBALMENU_APPLET_LIBS)

AC_SUBST(TEST_SUITE_CFLAGS)
AC_SUBST(TEST_SUITE_LIBS)


AC_SUBST(DIA)

AC_CHECK_PROGS(DIA, dia)

GTK_DOC_CHECK([1.0])
GNOME_DOC_INIT

if test x$enable_gtk_doc = xyes ; then
	if test x$DIA = "x" ; then
		AC_MSG_ERROR([dia is needed to build the doc])
	fi
fi
AC_OUTPUT(
Makefile
libgnomenu/Makefile
contrib/Makefile
doc/Makefile
doc/reference/Makefile
doc/reference/libgnomenu/Makefile
libgnomenu/libgnomenu.pc
gnome-globalmenu-applet/Makefile
test/Makefile
)
