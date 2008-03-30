AC_INIT(gnome-globalmenu, 0.5)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
GNOME_COMMON_INIT
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE
GNOME_MAINTAINER_MODE_DEFINES

IT_PROG_INTLTOOL([0.35.0])

LIBGNOMENU_LT_VERSION=0:0:1
AC_SUBST(LIBGNOMENU_LT_VERSION)
LIBGNOMENU_VERSION=0x00050001

AC_DEFINE_UNQUOTED([LIBGNOMENU_VERSION], $LIBGNOMENU_VERSION, [Version])
AC_SUBST(LIBGNOMENU_VERSION)

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL

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

PKG_CHECK_MODULES(LIBGNOMENU, [gtk+-2.0 >= 2.10 gdk-2.0])
PKG_CHECK_MODULES(TEST_SUITE, [gtk+-2.0 >= 2.10, gdk-2.0])
AC_SUBST(LIBGNOMENU_CFLAGS)
AC_SUBST(LIBGNOMENU_LIBS)
AC_SUBST(TEST_SUITE_CFLAGS)
AC_SUBST(TEST_SUITE_LIBS)
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
dnl if test "$enable_gtk_doc" = "yes" ; then
dnl	AC_SUBST(DIA)
dnl	AC_CHECK_PROGS(DIA, dia)
dnl	if test x$DIA = "x" ; then
dnl		AC_MSG_ERROR([dia is needed to build the doc])
dnl	fi
dnl fi

AC_OUTPUT(
Makefile
libgnomenu/Makefile
service/Makefile
manager/Makefile
gtk+-aqd/Makefile
po/Makefile.in
doc/Makefile
doc/reference/Makefile
doc/reference/libgnomenu/Makefile
libgnomenu/libgnomenu.pc
test/Makefile
)
