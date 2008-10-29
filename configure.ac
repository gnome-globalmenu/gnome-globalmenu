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

AC_PATH_PROG(VALAC_BIN, valac, valac, [`pwd`/vala$PATH_SEPARATOR$PATH])
VALAC='$(VALAC_BIN) $(VALAC_FLAGS)'
VALAC_FLAGS='--vapidir=$(top_srcdir)/vapi-compat --pkg=valacompat $(AM_VALAC_FLAGS)'
AC_SUBST(VALAC_BIN)
AC_SUBST(VALAC)
AC_SUBST(VALAC_FLAGS)
AC_SUBST(AM_VALAC_FLAGS)

dnl begin: GMarkupDoc - base
PKG_CHECK_MODULES(GMARKUPDOC_BASE, [glib-2.0])
AC_SUBST(GMARKUPDOC_BASE_CFLAGS)
AC_SUBST(GMARKUPDOC_BASE_LIBS)
dnl begin: GMarkupDoc - list
PKG_CHECK_MODULES(GMARKUPDOC_LIST, [glib-2.0, gtk+-2.0])
AC_SUBST(GMARKUPDOC_LIST_CFLAGS)
AC_SUBST(GMARKUPDOC_LIST_LIBS)
GMARKUPDOC_LIST_VALA_PKGS="--pkg gtk+-2.0"
AC_SUBST(GMARKUPDOC_LIST_VALA_PKGS)
dnl begin: GMarkupDoc - dbus
PKG_CHECK_MODULES(GMARKUPDOC_DBUS, [glib-2.0, gtk+-2.0, dbus-glib-1])
AC_SUBST(GMARKUPDOC_DBUS_CFLAGS)
AC_SUBST(GMARKUPDOC_DBUS_LIBS)
GMARKUPDOC_DBUS_VALA_PKGS="--pkg dbus-glib-1 --pkg gtk+-2.0"
AC_SUBST(GMARKUPDOC_DBUS_VALA_PKGS)

dnl begin: Gnomenu
PKG_CHECK_MODULES(GNOMENU, [glib-2.0 dbus-glib-1 gtk+-2.0])
AC_SUBST(GNOMENU_CFLAGS)
AC_SUBST(GNOMENU_LIBS)
GNOMENU_VALA_PKGS="--pkg dbus-glib-1 --pkg gtk+-2.0"
AC_SUBST(GNOMENU_VALA_PKGS)

dnl begin: GnomenuGTK
PKG_CHECK_MODULES(GNOMENUGTK, [glib-2.0 dbus-glib-1 gtk+-2.0 gio-2.0 gio-unix-2.0])
AC_SUBST(GNOMENUGTK_CFLAGS)
AC_SUBST(GNOMENUGTK_LIBS)
GNOMENUGTK_VALA_PKGS="--pkg dbus-glib-1 --pkg gtk+-2.0 --pkg gio-2.0 --pkg gio-unix-2.0"
AC_SUBST(GNOMENUGTK_VALA_PKGS)

dnl begin:applet
AC_MSG_CHECKING(--enable-gnome-applet argument)
AC_ARG_ENABLE(gnome-applet,
	[	--enable-gnome-applet	Include gnome-applet interface ],
	[	enable_gnome_applet=$enableval ],
	[	enable_gnome_applet="yes" ])
AC_DEFINE([ENABLE_PANEL_APPLET], [], [enable gnome applet])
AM_CONDITIONAL([ENABLE_PANEL_APPLET], [test "$enable_gnome_applet" = "yes" ])
AC_MSG_RESULT($enable_gnome_applet)

if test "$enable_gnome_applet" = "yes"; then
PKG_CHECK_MODULES(GLOBALMENU_PANELAPPLET, [glib-2.0 dbus-glib-1 gtk+-2.0 libwnck-1.0 libpanelapplet-2.0])
fi
AC_SUBST(GLOBALMENU_PANELAPPLET_CFLAGS)
AC_SUBST(GLOBALMENU_PANELAPPLET_LIBS)
GLOBALMENU_PANELAPPLET_VALA_PKGS="--pkg dbus-glib-1 --pkg gtk+-2.0 --pkg libwnck-1.0 --pkg libpanelapplet-2.0"
AC_SUBST(GLOBALMENU_PANELAPPLET_VALA_PKGS)

BONOBO_SERVER_DIR="$libdir/bonobo/servers"
AC_SUBST(BONOBO_SERVER_DIR)
AC_DEFINE_UNQUOTED(BONOBO_SERVER_DIR, "$BONOBO_SERVER_DIR", [Where server dir for BONOBO is])

dnl end:applet

VALA_VAPI_DIR="$datadir/vala/vapi"
AC_SUBST(VALA_VAPI_DIR)

dnl begin:gettext
GETTEXT_PACKAGE=gnome-globalmenu
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Define the gettext package to be used])

AM_GLIB_GNU_GETTEXT
GNOME_GLOBALMENU_LOCALEDIR=[${datadir}/locale]
AC_SUBST(GNOME_GLOBALMENU_LOCALEDIR)
dnl end:gettext

dnl org.gnome.GlobalMenu.Server
PKG_CHECK_MODULES(ORG_GNOME_GLOBALMENU_SERVER, [glib-2.0 dbus-glib-1 gtk+-2.0])
AC_SUBST(ORG_GNOME_GLOBALMENU_SERVER_CFLAGS)
AC_SUBST(ORG_GNOME_GLOBALMENU_SERVER_LIBS)
ORG_GNOME_GLOBALMENU_SERVER_VALA_PKGS="--pkg dbus-glib-1 --pkg gtk+-2.0"
AC_SUBST(ORG_GNOME_GLOBALMENU_SERVER_VALA_PKGS)

DBUS_SERVICES_DIR="$datadir/dbus-1/services"
AC_SUBST(DBUS_SERVICES_DIR)
AC_DEFINE_UNQUOTED(DBUS_SERVICES_DIR, "$DBUS_SERVICES_DIR", [Where services dir for DBUS is])

dnl gnomenu-navigator
PKG_CHECK_MODULES(GNOMENU_NAVIGATOR, [glib-2.0 dbus-glib-1 gtk+-2.0])
AC_SUBST(GNOMENU_NAVIGATOR_CFLAGS)
AC_SUBST(GNOMENU_NAVIGATOR_LIBS)
GNOMENU_NAVIGATOR_VALA_PKGS="--pkg dbus-glib-1 --pkg gtk+-2.0"
AC_SUBST(GNOMENU_NAVIGATOR_VALA_PKGS)

AM_GCONF_SOURCE_2

dnl begin: glade
dnl end: glade

GTK_DOC_CHECK([1.0])
GNOME_DOC_INIT

AC_OUTPUT(
Makefile
vapi-compat/Makefile
libgmarkupdoc/Makefile
libgnomenu/Makefile
gtk-modules/Makefile
gtk-modules/libgnomenugtk/Makefile
server/Makefile
navigator/Makefile
tests/Makefile
applet/Makefile
po/Makefile.in
)
