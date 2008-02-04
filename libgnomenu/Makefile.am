#

INCLUDES = $(LIBGNOMENU_CFLAGS)

lib_LTLIBRARIES = libgnomenu.la

BUILT_SOURCES = gnomenu-marshall.h gnomenu-marshall.c gnomenu-enums.c gnomenu-enums.h

libgnomenu_la_SOURCES = \
	$(BUILT_SOURCES) \
	gnomenumessage.h\
	gnomenumessage.c\
	gdksocket.h \
	gdksocket.c \
	serverhelper.c \
	serverhelper.h \
	clienthelper.c \
	clienthelper.h \
	quirks.h	\
	quirks.c

libgnomenu_la_LDFLAGS = \
	-version-info=$(LIBGNOMENU_LT_VERSION) \
	-no-undefined \
	-no-allow-shlib-undefined

libgnomenu_la_LIBADD = $(LIBGNOMENU_LIBS)

$(libgnomenu_la_OBJECTS) : $(BUILT_SOURCES)


libgnomenuincludedir = $(includedir)/libgnomenu
libgnomenuinclude_HEADERS = gdksocket.h serverhelper.h gnomenumessage.h clienthelper.h quirks.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libgnomenu.pc

EXTRA_DIST = gnomenu-marshall.list

gnomenu-marshall.c: gnomenu-marshall.list
	glib-genmarshal --internal --body --prefix=gnomenu_marshall gnomenu-marshall.list > $@
gnomenu-marshall.h: gnomenu-marshall.list
	glib-genmarshal --internal --header --prefix=gnomenu_marshall gnomenu-marshall.list > $@

gnomenu_enum_headers = gnomenumessage.h

gnomenu-enums.c: $(gnomenu_enum_headers) 
	glib-mkenums --fhead "#include <glib-object.h>\n" \
             --fhead "#include \"gnomenu-enums.h\"\n\n" \
             --fprod "\n/* enumerations from \"@filename@\" */" \
             --fprod "\n#include \"@filename@\"\n" \
             --vhead "static const G@Type@Value _@enum_name@_values[] = {" \
             --vprod "  { @VALUENAME@, \"@VALUENAME@\", \"@valuenick@\" }," \
             --vtail "  { 0, NULL, NULL }\n};\n\n" \
             --vtail "GType\n@enum_name@_get_type (void)\n{\n" \
             --vtail "  static GType type = 0;\n\n" \
             --vtail "  if (!type)\n" \
             --vtail "    type = g_@type@_register_static (\"@EnumName@\", _@enum_name@_values);\n\n" \
             --vtail "  return type;\n}\n\n" \
        $(gnomenu_enum_headers) > $@ 
 
gnomenu-enums.h: $(gnomenu_enum_headers) 
	glib-mkenums --fhead "#ifndef __GNOMENU_ENUMS_H__\n" \
             --fhead "#define __GNOMENU_ENUMS_H__\n\n" \
             --fhead "G_BEGIN_DECLS\n\n" \
             --ftail "G_END_DECLS\n\n" \
             --ftail "#endif /* __GNOMENU_ENUMS_H__ */\n" \
             --fprod "\n/* --- @filename@ --- */" \
             --eprod "#define GNOMENU_TYPE_@ENUMSHORT@ @enum_name@_get_type()\n" \
             --eprod "GType @enum_name@_get_type (void);\n" \
        $(gnomenu_enum_headers) > $@ 


CLEANFILES = 
#$(BUILT_SOURCES)
dist-hook:
	cd $(distdir) ; rm -f $(CLEANFILES)
