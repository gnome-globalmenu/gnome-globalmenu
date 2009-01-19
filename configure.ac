AC_INIT(gnome-globalmenu, [0.7.2], [rainwoodman@gmail.com], [gnome-globalmenu])

AM_INIT_AUTOMAKE([1.9 tar-ustar])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS(config.h)

# maintainer mode
AM_MAINTAINER_MODE


# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_LIBTOOL_DLOPEN
AC_PROG_LIBTOOL
AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)

AM_GCONF_SOURCE_2

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

# Check for Vala
AC_SUBST(VALA_REQUIRED, [0.5.5])
AC_PATH_PROG(VALAC_BIN, [valac `pwd`/vala$PATH_SEPARATOR$PATH])

VALA_DEFAULT_PKGS="--vapidir \$(top_srcdir) --pkg configmake --pkg licenses --pkg config"
VALAC="$VALAC_BIN $VALA_DEFAULT_PKGS"
AC_SUBST(VALAC)
VALA_VERSION=`$VALAC_BIN --version|sed -e 's/[Vala ]*//'`
AC_SUBST(VALA_VERSION)

AC_CONFIG_COMMANDS_POST(
		[
	   	test x$VALA_VERSION == x$VALA_REQUIRED || 
		AC_MSG_WARN([
			This package works with VALA $VALA_REQUIRED;
			Your have $VALA_VERSION installed;
			You may experience compilation failure or
			random runtime failurs.
			])
		])

# check for modules

GLIB_REQUIRED=2.10.0
GTK_REQUIRED=2.10.0

WNCK_REQUIRED=2.16.0
PANEL_REQUIRED=2.16.0
GMENU_REQUIRED=2.16.0
NOTIFY_REQUIRED=0.4.0
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


AC_ARG_WITH(
	[gnome-panel],
	AC_HELP_STRING([--without-gnome-panel], [disable gnome panel applet]),
	[],
	[with_gnome_panel=yes])
AS_IF([test "x$with_gnome_panel" != xno],
	[PKG_CHECK_MODULES(PANEL, [libpanelapplet-2.0 >= $PANEL_REQUIRED, libnotify >= 0.4.0],
		[
		with_gnome_panel=true;
		AC_SUBST(PANEL_CFLAGS)
		AC_SUBST(PANEL_LIBS)
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
		],
		[
		with_xfce4_panel=false;
		if test "x$with_xfce4_panel" != xcheck; then
			AC_MSG_FAILURE(
				[test for libxfce4panel-1.0 failed. (--without-xfce4-panel to disable).])
		fi;]
	)])
AM_CONDITIONAL(WITH_XFCE4_PANEL, [test "x$with_xfce4_panel" = xtrue ]) 


PKG_CHECK_MODULES(GMENU, libgnome-menu >= $GMENU_REQUIRED)
AC_SUBST(GMENU_FLAGS)
AC_SUBST(GMENU_LIBS)

# path: bad way; move them back to the Makefiles according to GNU rules?

BONOBO_SERVER_DIR="$libdir/bonobo/servers"
AC_SUBST(BONOBO_SERVER_DIR)
AC_DEFINE_UNQUOTED(BONOBO_SERVER_DIR, "$BONOBO_SERVER_DIR", [Where server dir for BONOBO is])

GTK_MODULES_DIR="$libdir/gtk-2.0/modules"
AC_SUBST(GTK_MODULES_DIR)
AC_DEFINE_UNQUOTED(GTK_MODULES_DIR, "$GTK_MODULES_DIR", [Where the gtk-2.0/modules dir is])

# old automake fix
AC_SUBST(abs_top_srcdir)
# output

AC_CONFIG_FILES(
	[Makefile
	libgnomenu/Makefile
	libgnomenu/tests/Makefile
	applet/Makefile
	xfce/Makefile
	tests/Makefile
	tools/Makefile
	gtk-modules/Makefile
	gtk-modules/globalmenu/Makefile
	gtk-modules/globalmenu/tests/Makefile
	po/Makefile.in
	patch.sh
	])

AC_OUTPUT
