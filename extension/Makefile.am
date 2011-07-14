
GlobalMenuGnomeShell-1.0.typelib: GlobalMenuGnomeShell-1.0.gir
	$(INTROSPECTION_COMPILER) -o $@ $^

extensiondir = $(EXTDIR)/GlobalMenu@globalmenu.org
extension.js: extension.js.in
	sed "s;@EXTENSION_PATH@;$(extensiondir);" < $^ > $@

extension_DATA = extension.js metadata.json stylesheet.css GlobalMenuGnomeShell-1.0.typelib
