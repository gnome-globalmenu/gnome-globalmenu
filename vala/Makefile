VALAC_PATH=$(abspath $(dir valac))/vala-trunk

all: valac
vala-trunk.stamp:
	svn co http://svn.gnome.org/svn/vala/trunk vala-trunk
	touch vala-trunk/NO-AUTO-GEN
	touch vala-trunk.stamp
patch.stamp: vala-trunk.stamp
	cp *.patch vala-trunk/
	(cd vala-trunk; for i in *.patch ; do echo applying $$i; patch -p0 < $$i; done)
	touch patch.stamp	
autogen.stamp: patch.stamp
	(cd vala-trunk; ./autogen.sh --enable-vapigen)
	touch autogen.stamp
valac: vala-trunk.stamp patch.stamp autogen.stamp
	(cd vala-trunk; make)
	echo exec $(VALAC_PATH)/compiler/valac --vapidir=$(VALAC_PATH)/vapi '$$*' > valac
	chmod +x valac
