AC_INIT(gnome-globalmenu, [0.6.9], [rainwoodman@gmail.com], [gnome-globalmenu])

AM_INIT_AUTOMAKE([1.9 tar-ustar])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS(config.h)

# maintainer mode
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)

# Check for Vala
AC_SUBST(VALA_REQUIRED, [0.5.1])

AC_PATH_PROG(VALAC, [valac `pwd`/vala$PATH_SEPARATOR$PATH])
AC_SUBST(VALAC, [$ac_cv_path_VALAC])

AC_CONFIG_COMMANDS_POST(
		[
		VALA_VERSION=`$VALAC --version`
	   	echo $VALA_VERSION | grep $VALA_REQUIRED || \
		AC_MSG_WARN([
			This package works with VALA $VALA_REQUIRED;
			Your have $VALA_VERSION installed;
			You may experience compilation failure or
			random runtime failurs.
			])
		])

# check for modules

GLIB_REQUIRED=2.10.0
GTK_REQUIRED=2.12.0
GIO_REQUIRED=2.12.0

WNCK_REQUIRED=2.24.1
PANEL_REQUIRED=2.24.1

PKG_CHECK_MODULES(GLIB, glib-2.0 >= $GLIB_REQUIRED gobject-2.0 >= $GLIB_REQUIRED)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

PKG_CHECK_MODULES(GTK, gtk+-2.0 >= $GTK_REQUIRED)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

PKG_CHECK_MODULES(WNCK, libwnck-1.0 >= $WNCK_REQUIRED)
AC_SUBST(WNCK_CFLAGS)
AC_SUBST(WNCK_LIBS)

PKG_CHECK_MODULES(PANEL, libpanelapplet-2.0 >= $PANEL_REQUIRED)
AC_SUBST(PANEL_CFALGS)
AC_SUBST(PANEL_LIBS)

PKG_CHECK_MODULES(GIO, gio-2.0 >= $GIO_REQUIRED gio-unix-2.0 >= $GIO_REQUIRED)
AC_SUBST(GIO_FLAGS)
AC_SUBST(GIO_LIBS)

# output

AC_CONFIG_FILES(
	[Makefile
	libgnomenu/Makefile
	libgnomenu/tests/Makefile
	applet/Makefile
	tests/Makefile
	gtk-modules/Makefile
	gtk-modules/globalmenu/Makefile
	])

AC_OUTPUT
