AC_INIT(gnome-globalmenu, [0.6.9], [rainwoodman@gmail.com], [gnome-globalmenu])

AM_INIT_AUTOMAKE([1.9 tar-ustar])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS(config.h)

# maintainer mode
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CC
#AM_PROG_CC_C_O
#AC_DISABLE_STATIC
AC_PROG_LIBTOOL
AC_SUBST(CFLAGS)
AC_SUBST(CPPFLAGS)
AC_SUBST(LDFLAGS)

# Check for Vala
AC_SUBST(VALA_REQUIRED, [0.5.1])

AC_CACHE_CHECK([checking for vala = $VALA_REQUIRED],
		[ac_cv_path_VALAC],
		[AC_PATH_PROGS_FEATURE_CHECK([VALAC], 
			[valac valac [`pwd`/vala$PATH_SEPARATOR$PATH]],
			[[
			$ac_path_VALAC --version | grep $VALA_REQUIRED &&
				ac_cv_path_VALAC=$ac_path_VALAC ac_path_VALAC_found=: 
			]],
			[AC_MSG_ERROR([could not find vala version $VALA_REQUIRED])]
			)]
		)
AC_SUBST(VALAC, [$ac_cv_path_VALAC])

# check for modules

GLIB_REQUIRED=2.10.0
GTK_REQUIRED=2.12.0

PKG_CHECK_MODULES(GLIB, glib-2.0 >= $GLIB_REQUIRED gobject-2.0 >= $GLIB_REQUIRED)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

PKG_CHECK_MODULES(GTK, gtk+-2.0 >= $GTK_REQUIRED)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

# output

AC_CONFIG_FILES(
	[Makefile
	libgnomenu/Makefile
	libgnomenu/tests/Makefile
	tests/Makefile
	])

AC_OUTPUT
