dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(gnome2-globalmenu-applet, 0.2)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC




dnl ***************************************************************************
dnl Internatinalization
dnl ***************************************************************************
GETTEXT_PACKAGE=gnome2-globalmenu-applet
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])
AM_GLIB_GNU_GETTEXT
IT_PROG_INTLTOOL([0.35.0])



AM_PROG_LIBTOOL



PKG_CHECK_MODULES(GNOME2_GLOBALMENU_APPLET, [gtk+-2.0 >= 2.8 libwnck-1.0 libpanelapplet-2.0 gdk-2.0])
AC_SUBST(GNOME2_GLOBALMENU_APPLET_CFLAGS)
AC_SUBST(GNOME2_GLOBALMENU_APPLET_LIBS)




PKG_CHECK_MODULES(TEST_CLIENT, gtk+-2.0 libglade-2.0 )
AC_SUBST(TEST_CLIENT_CFLAGS)
AC_SUBST(TEST_CLIENT_LIBS)

AC_OUTPUT([
Makefile
src/Makefile
po/Makefile.in
])
