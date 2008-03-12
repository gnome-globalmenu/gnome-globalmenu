AC_INIT(gnome-globalmenu, 0.4)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
GNOME_COMMON_INIT
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE
GNOME_MAINTAINER_MODE_DEFINES

IT_PROG_INTLTOOL([0.35.0])

LIBGNOMENU_LT_VERSION=0:0:1
AC_SUBST(LIBGNOMENU_LT_VERSION)
AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL

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

dnl Servers;

AC_MSG_CHECKING(--enable-standalone argument)
AC_ARG_ENABLE(standalone,
	[	--enable-standalone	Include standalone menu server ],
	[	enable_standalone=$enableval ],
	[	enable_standalone="yes" ])
AC_DEFINE([ENABLE_STANDALONE], [], [enable stand alone])
AM_CONDITIONAL([ENABLE_STANDALONE], [test "$enable_standalone" = "yes" ])
AC_MSG_RESULT($enable_standalone)

AC_MSG_CHECKING(--enable-gnome-applet argument)
AC_ARG_ENABLE(gnome-applet,
	[	--enable-gnome-applet	Include gnome-applet interface ],
	[	enable_gnome_applet=$enableval ],
	[	enable_gnome_applet="yes" ])
AC_DEFINE([ENABLE_GNOME_APPLET], [], [enable gnome applet])
AM_CONDITIONAL([ENABLE_GNOME_APPLET], [test "$enable_gnome_applet" = "yes" ])
AC_MSG_RESULT($enable_gnome_applet)

AC_MSG_CHECKING(--enable-xfce-plugin argument)
AC_ARG_ENABLE(xfce-plugin,
	[	--enable-xfce-plugin	Include xfce-plugin interface ],
	[	enable_xfce_plugin=$enableval ],
	[	enable_xfce_plugin="yes" ])
AC_DEFINE([ENABLE_XFCE_PLUGIN], [], [enable xfce plugin])
AM_CONDITIONAL([ENABLE_XFCE_PLUGIN], [test "$enable_xfce_plugin" = "yes" ])
AC_MSG_RESULT($enable_xfce_plugin)

PKG_CHECK_MODULES(LIBGNOMENU, [gtk+-2.0 >= 2.10 gdk-2.0])
if test "$enable_standalone" = "yes"; then
	PKG_CHECK_MODULES(GLOBALMENU_SERVER, [gtk+-2.0 >= 2.10, gdk-2.0, libwnck-1.0] )
	AC_SUBST(GLOBALMENU_SERVER_CFLAGS)
	AC_SUBST(GLOBALMENU_SERVER_LIBS)
	AC_DEFINE(ENABLE_STANDALONE)
fi
if test "$enable_gnome_applet" = "yes"; then
	PKG_CHECK_MODULES(GNOME_GLOBALMENU_APPLET, [gtk+-2.0 >= 2.10, gdk-2.0, libwnck-1.0, libpanelapplet-2.0 bonobo-activation-2.0])
	AC_SUBST(GNOME_GLOBALMENU_APPLET_CFLAGS)
	AC_SUBST(GNOME_GLOBALMENU_APPLET_LIBS)
	AC_DEFINE(ENABLE_GNOME_APPLET)
fi
if test "$enable_xfce_plugin" = "yes"; then
	PKG_CHECK_MODULES(XFCE_GLOBALMENU_PLUGIN, [gtk+-2.0 >= 2.10, gdk-2.0, libwnck-1.0, libxfce4panel-1.0])
	AC_SUBST(XFCE_GLOBALMENU_PLUGIN_CFLAGS)
	AC_SUBST(XFCE_GLOBALMENU_PLUGIN_LIBS)
	AC_DEFINE(ENABLE_XFCE_PLUGIN)
fi

PKG_CHECK_MODULES(TEST_SUITE, [gtk+-2.0 >= 2.10, gdk-2.0])

AC_SUBST(LIBGNOMENU_CFLAGS)
AC_SUBST(LIBGNOMENU_LIBS)



AC_SUBST(TEST_SUITE_CFLAGS)
AC_SUBST(TEST_SUITE_LIBS)


GETTEXT_PACKAGE=gnome-globalmenu
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Define the gettext package to be used])

AM_GLIB_GNU_GETTEXT
GNOME_GLOBALMENU_LOCALEDIR=[${datadir}/locale]
AC_SUBST(GNOME_GLOBALMENU_LOCALEDIR)

AM_GCONF_SOURCE_2


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
po/Makefile.in
gtk+-aqd/Makefile
doc/Makefile
doc/reference/Makefile
doc/reference/libgnomenu/Makefile
libgnomenu/libgnomenu.pc
servers/Makefile
test/Makefile
)
