AC_INIT(gnome-globalmenu, [0.6], [rainwoodman@gmail.com], [gnome-globalmenu] )

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
GNOME_COMMON_INIT
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE
GNOME_MAINTAINER_MODE_DEFINES

IT_PROG_INTLTOOL([0.35.0])

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL

AC_PATH_PROG(VALAC, valac, valac, [`pwd`/vala$PATH_SEPARATOR$PATH])
AC_SUBST(VALAC)

dnl begin: tracing
AC_MSG_CHECKING(--enable-tracing arguement)
AC_ARG_ENABLE(tracing,
	[	--enable-tracing Enable many debugging information (1,2,no)],
	[	enable_tracing=$enableval ],
	[	enable_tracing="1"	])
AC_MSG_RESULT($enable_tracing)

if test "$enable_tracing" == "no"; then
	AC_DEFINE([ENABLE_TRACING], 0, [enable tracing])
fi
if test "$enable_tracing" == "1"; then
	AC_DEFINE([ENABLE_TRACING], 1, [enable tracing])
fi
if test "$enable_tracing" == "2"; then
	AC_DEFINE([ENABLE_TRACING], 2, [enable tracing])
fi
if test "$enable_tracing" == "3"; then
	AC_DEFINE([ENABLE_TRACING], 3, [enable tracing])
fi
dnl end:tracing

dnl begin:libgnomenu
PKG_CHECK_MODULES(LIBGNOMENU, [glib-2.0 dbus-glib-1 gtk+-2.0])
AC_SUBST(LIBGNOMENU_CFLAGS)
AC_SUBST(LIBGNOMENU_LIBS)
LIBGNOMENU_VALA_PKGS="--pkg dbus-glib-1 --pkg gtk+-2.0"
AC_SUBST(LIBGNOMENU_VALA_PKGS)
dnl end:libgnomenu

dnl begin:tests
PKG_CHECK_MODULES(TESTS, [glib-2.0 dbus-glib-1 gtk+-2.0])
AC_SUBST(TESTS_CFLAGS)
AC_SUBST(TESTS_LIBS)
TESTS_VALA_PKGS="--pkg dbus-glib-1 --pkg gtk+-2.0"
AC_SUBST(TESTS_VALA_PKGS)
dnl end:libgnomenu

dnl begin:gettext
GETTEXT_PACKAGE=gnome-globalmenu
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Define the gettext package to be used])

AM_GLIB_GNU_GETTEXT
GNOME_GLOBALMENU_LOCALEDIR=[${datadir}/locale]
AC_SUBST(GNOME_GLOBALMENU_LOCALEDIR)
dnl end:gettext

AM_GCONF_SOURCE_2

dnl begin: glade
dnl end: glade

GTK_DOC_CHECK([1.0])
GNOME_DOC_INIT

AC_OUTPUT(
Makefile
libgnomenu/Makefile
libgnomenu/tests/Makefile
gtk-aqd/Makefile
po/Makefile.in
)
