AC_INIT(gnome-globalmenu, [0.7.4], [rainwoodman@gmail.com], [gnome-globalmenu])

AC_CONFIG_MACRO_DIR([autotools])
AC_CONFIG_AUX_DIR([autotools])
AC_SUBST(ACLOCAL_AMFLAGS, "-I autotools")

AM_INIT_AUTOMAKE([1.9 tar-ustar dist-bzip2])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS(config.h)

# maintainer mode
AM_MAINTAINER_MODE


# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_LIBTOOL_DLOPEN
AC_DISABLE_STATIC
AC_PROG_LIBTOOL
DOLT
AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)

AM_GCONF_SOURCE_2

AC_PATH_PROG(GCONFTOOL, gconftool-2)

# tests
USE_TESTS=true
AC_ARG_ENABLE(
	[tests],
	AC_HELP_STRING([--enable-tests], [enable tests, default is yes]),
	[ test "x$enableval" == xno && USE_TESTS=false ],
	[ USE_TESTS=true ])
AM_CONDITIONAL([ENABLE_TESTS], [test "x$USE_TESTS" == xtrue])

# intl
IT_PROG_INTLTOOL
AM_GLIB_GNU_GETTEXT
GETTEXT_PACKAGE=gnome-globalmenu
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, ["$GETTEXT_PACKAGE"], [gettext package])
AC_SUBST(GETTEXT_PACKAGE)
if test "x$localedir" == x; then
localedir='${datadir}/locale'
AC_SUBST(localedir)
fi
# Check for Vala
VALA_PROG_VALAC(0.5.6)

VALA_DEFAULT_PKGS="--vapidir \$(top_srcdir) --vapidir \$(top_srcdir)/vapi --pkg configmake --pkg licenses --pkg config"
AC_SUBST(VALA_DEFAULT_PKGS)
# check for modules

GLIB_REQUIRED=2.10.0
GTK_REQUIRED=2.10.0

WNCK_REQUIRED=2.16.0
PANEL_REQUIRED=2.16.0
GMENU_REQUIRED=2.16.0
NOTIFY_REQUIRED=0.4.0
X11_REQUIRED=1.1.0
XFCE4_PANEL_REQUIRED=4.4.3

PKG_CHECK_MODULES(GLIB, glib-2.0 >= $GLIB_REQUIRED gobject-2.0 >= $GLIB_REQUIRED gmodule-2.0 >= $GLIB_REQUIRED)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

PKG_CHECK_MODULES(GTK, gtk+-2.0 >= $GTK_REQUIRED)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

PKG_CHECK_MODULES(WNCK, libwnck-1.0 >= $WNCK_REQUIRED)
AC_SUBST(WNCK_CFLAGS)
AC_SUBST(WNCK_LIBS)

PKG_CHECK_MODULES(GMENU, libgnome-menu >= $GMENU_REQUIRED)
AC_SUBST(GMENU_CFLAGS)
AC_SUBST(GMENU_LIBS)

PKG_CHECK_MODULES(X11, x11 >= $X11_REQUIRED)
AC_SUBST(X11_CFLAGS)
AC_SUBST(X11_LIBS)

AC_ARG_WITH(
	[gir],
	AC_HELP_STRING([--with-gir], [enable building gir typelib]),
	[],
	[with_gir=no])
AS_IF([test "x$with_gir" != xno],
	[
	AC_PATH_PROG(GIRCOMPILER, [g-ir-compiler] )
	AS_IF([test -n "$GIRCOMPILER" ],
		[with_gir=true],
		AC_MSG_FAILURE(
			[test for g-ir-compiler failed. (--without-gir to disable).]
		)
	)
	AC_SUBST(GIRCOMPILER)
	GIRDIR=`$PKG_CONFIG --variable=girdir gobject-introspection-1.0`
	AC_SUBST(GIRDIR)
	TYPELIBDIR="$($PKG_CONFIG --variable=typelibdir gobject-introspection-1.0)"
	AC_SUBST(TYPELIBDIR)
	]
)
AM_CONDITIONAL(WITH_GIR, [test "x$with_gir" = xtrue ]) 
AC_ARG_WITH(
	[gnome-panel],
	AC_HELP_STRING([--without-gnome-panel], [disable gnome panel applet]),
	[],
	[with_gnome_panel=yes])
AS_IF([test "x$with_gnome_panel" != xno],
	[PKG_CHECK_MODULES(GNOME_PANEL, 
		[	libpanelapplet-2.0 >= $PANEL_REQUIRED, 
			libnotify >= $NOTIFY_REQUIRED
		],
		[
		with_gnome_panel=true;
		AC_SUBST(GNOME_PANEL_CFLAGS)
		AC_SUBST(GNOME_PANEL_LIBS)
		BONOBO_SERVER_DIR="$($PKG_CONFIG --variable=libdir bonobo-activation-2.0)/bonobo/servers"
		AC_SUBST(BONOBO_SERVER_DIR)
		],
		[
		with_gnome_panel=false;
		if test "x$with_gnome_panel" != xcheck; then
			AC_MSG_FAILURE(
				[test for libpanelapplet-2.0 and libnotify failed. (--without-gnome-panel to disable).])
		fi;]
	)])
AM_CONDITIONAL(WITH_GNOME_PANEL, [test "x$with_gnome_panel" = xtrue ]) 

AC_ARG_WITH(
	[xfce4-panel],
	AC_HELP_STRING([--with-xfce4-panel], [enable xfce panel applet]),
	[],
	[with_xfce4_panel=no])
AS_IF([test "x$with_xfce4_panel" != xno],
	[PKG_CHECK_MODULES(XFCE4_PANEL, [libxfce4panel-1.0 >= $XFCE4_PANEL_REQUIRED],
		[
		with_xfce4_panel=true;
		AC_SUBST(XFCE4_PANEL_CFLAGS)
		AC_SUBST(XFCE4_PANEL_LIBS)
		XFCE4_PANEL_PLUGIN_DIR="$($PKG_CONFIG --variable=libexecdir libxfce4panel-1.0)/xfce4/panel-plugins"
		XFCE4_PANEL_DESKTOP_DIR="$($PKG_CONFIG --variable=datadir libxfce4panel-1.0)/xfce4/panel-plugins"
		AC_SUBST(XFCE4_PANEL_PLUGIN_DIR)
		AC_SUBST(XFCE4_PANEL_DESKTOP_DIR)
		],
		[
		with_xfce4_panel=false;
		if test "x$with_xfce4_panel" != xcheck; then
			AC_MSG_FAILURE(
				[test for libxfce4panel-1.0 failed. (--without-xfce4-panel to disable).])
		fi;]
	)])
AM_CONDITIONAL(WITH_XFCE4_PANEL, [test "x$with_xfce4_panel" = xtrue ]) 


GTK_MODULES_DIR="$($PKG_CONFIG --variable=libdir gtk+-2.0)/gtk-2.0/modules"
AC_SUBST(GTK_MODULES_DIR)

# svnversion
AC_PATH_PROG(SVNVERSION_BIN, [svnversion])
SVNVERSION=`$SVNVERSION_BIN|sed -e 's;:;_;g' -e 's;exported;;'`
AC_SUBST(SVNVERSION)
AC_DEFINE_UNQUOTED(SVNVERSION, "$SVNVERSION", [svn version if appropriate])

# old automake fix
AC_SUBST(abs_top_srcdir)
# output

AC_CONFIG_FILES(
	[Makefile
	libcompat/Makefile
	libgnomenu/Makefile
	libgnomenu/tests/Makefile
	libgnomenu/libgnomenu.pc
	libgnomenu/libgnomenu-uninstalled.pc
	applet/Makefile
	xfce/Makefile
	tests/Makefile
	tools/Makefile
	vapi/Makefile
	gtk-modules/Makefile
	gtk-modules/globalmenu/Makefile
	gtk-modules/globalmenu/tests/Makefile
	po/Makefile.in
	patch.sh
	])

AC_OUTPUT
