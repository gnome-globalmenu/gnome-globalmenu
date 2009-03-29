dnl Autoconf scripts for the Vala compiler
dnl Copyright (C) 2007  Mathias Hasselmann
dnl
dnl This library is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU Lesser General Public
dnl License as published by the Free Software Foundation; either
dnl version 2 of the License, or (at your option) any later version.

dnl This library is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl Lesser General Public License for more details.

dnl You should have received a copy of the GNU Lesser General Public
dnl License along with this library; if not, write to the Free Software
dnl Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
dnl
dnl Author:
dnl 	Mathias Hasselmann <mathias.hasselmann@gmx.de>
dnl Modified by:
dnl 	Yu Feng
dnl --------------------------------------------------------------------------

dnl VALA_PROG_VALAC([MINIMUM-VERSION])
dnl
dnl Check whether the Vala compiler exists in `PATH'. If it is found the
dnl variable VALAC is set. Optionally a minimum release number of the compiler
dnl can be requested.
dnl --------------------------------------------------------------------------
AC_DEFUN([VALA_PROG_VALAC],[
  AC_PATH_PROG([VALAC_BIN], [valac], [])
  AC_SUBST(VALAC_BIN)
  VALAC="$VALAC_BIN --vapidir=\$(top_srcdir)/vapi"
  AC_SUBST(VALAC)
dnl  these are not useful since autoconf doesn't allow nested
dnl  substitutions in _SOURCES
dnl VALA_CHEADERS='$(VALASOURCES:.vala=.h)'
dnl    'VALA_CSOURCES=$(VALASOURCES:.vala=.c)' \
dnl   'VALA_CCODE=$(VALA_CHEADERS) $(VALACSOURCES)' \
dnl    'VALA_OBJECTS=$(VALASOURCES:.vala=.o)'

dnl    AC_SUBST(VALA_CSOURCES)
dnl AC_SUBST(VALA_CCODE_HEADERS)
dnl    AC_SUBST(VALA_CCODE)
dnl    AC_SUBST(VALA_OBJECTS)
  VALA_CCODE_RULES='vala-ccode: $(VALASOURCES); $(VALAC) $(VALAFLAGS) -C $^ $(VALAPKGS) && touch vala-ccode'
  VALA_OBJECT_RULES='vala-object: $(VALASOURCES); $(VALAC) $(VALAFLAGS) -c $^ $(VALAPKGS) && touch vala-object'

  AC_SUBST(VALA_OBJECT_RULES)
  AC_SUBST(VALA_CCODE_RULES)

  if test -z "x${VALAC_BIN}"; then
    AC_MSG_WARN([No Vala compiler found. You will not be able to recompile .vala source files.])
  elif test -n "x$1"; then
    AC_REQUIRE([AC_PROG_AWK])
    AC_MSG_CHECKING([valac is at least version $1])

    if "${VALAC_BIN}" --version | "${AWK}" -v r='$1' 'function vn(s) { if (3 == split(s,v,".")) return (v[1]*1000+v[2])*1000+v[3]; else exit 2; } /^Vala / { exit vn(r) > vn($[2]) }'; then
      AC_MSG_RESULT([yes])
    else
      AC_MSG_RESULT([no])
      AC_MSG_ERROR([Vala $1 not found.])
    fi
  fi
])
