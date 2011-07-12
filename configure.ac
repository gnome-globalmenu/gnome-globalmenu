AC_INIT(gnome-globalmenu, [0.9], [rainwoodman@gmail.com], [gnome-globalmenu])

AC_CONFIG_MACRO_DIR([autotools])
AC_CONFIG_AUX_DIR([autotools])
AC_SUBST(ACLOCAL_AMFLAGS, "-I autotools")

AM_INIT_AUTOMAKE([1.11 tar-ustar dist-bzip2 subdir-objects])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS(config.h)

# maintainer mode
AM_MAINTAINER_MODE


# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AM_PROG_VALAC([0.12.0])
AC_LIBTOOL_DLOPEN
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)

# intl
AM_GLIB_GNU_GETTEXT
# The following lines replaces glib-gettext with autopoint which doesn't work with GNOME
#AM_GNU_GETTEXT(external)
#AM_GNU_GETTEXT_VERSION(0.14.5)
IT_PROG_INTLTOOL
GETTEXT_PACKAGE=gnome-globalmenu
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, ["$GETTEXT_PACKAGE"], [gettext package])
AC_SUBST(GETTEXT_PACKAGE)
if test "x$localedir" == x; then
localedir='${datadir}/locale'
AC_SUBST(localedir)
fi

GOBJECT_INTROSPECTION_CHECK([0.6.7])

GLIB_REQUIRED=2.28.0
GTK_REQUIRED=3.0.3

PKG_CHECK_MODULES(GLIB, glib-2.0 >= $GLIB_REQUIRED gobject-2.0 >= $GLIB_REQUIRED gmodule-2.0 >= $GLIB_REQUIRED gio-2.0 >= $GLIB_REQUIRED)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

PKG_CHECK_MODULES(GTK, gtk+-3.0 >= $GTK_REQUIRED)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

GTK_MODULES_DIR="$($PKG_CONFIG --variable=libdir gtk+-2.0)/gtk-2.0/modules"
AC_SUBST(GTK_MODULES_DIR)

# output

AC_CONFIG_FILES(
	[Makefile
	globalmenu-plugin/Makefile
	src/Makefile
	po/Makefile.in
	])

AC_OUTPUT
