GTK = GTK_2_12_8
REV = 19551
get-gtk:
	git-svn --clone svn://svn.gnome.org/svn/gtk+/tags/$(GTK) -r $(REV)
patch-gtk:
	for i in *.patch; do\
		ln -s ../$$i $(GTK)/;\
		(cd $(GTK); patch -p1 < $$i; ) \
	done;
