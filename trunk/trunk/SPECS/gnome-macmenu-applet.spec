Summary: Gnome Macmenu Applet
Name: gnome-macmenu-applet
version: 1.0.14
Release: 5
Group: System
License: LGPL
source: http://aur.archlinux.org/packages/gnome-macmenu-applet/gnome-macmenu-applet.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-root
Patch0: gnome-macmenu-applet.patch
Patch1: gnome-macmenu-applet-scrollmenu.patch
Requires: gtk2
Requires: gnome-panel
Requires: libwnck

%description
Gnome mac menubar applet moves your menubar into a panel applet.

%prep
%setup -q -n %{name}
%patch0  -p1
%patch1  -p1

%build
gcc -std=gnu99 -Wall -fno-strict-aliasing -DFOR_GNOME `pkg-config --cflags --libs libwnck-1.0 libpanelapplet-2.0 gconf-2.0` $CFLAGS $LDFLAGS -o gnome-macmenu-applet macmenu-applet.c

%install
install -d -m 755 %{buildroot}%{_libexecdir}
install -d -m 755 %{buildroot}%{_libdir}/bonobo/servers
install -m 755 gnome-macmenu-applet %{buildroot}%{_libexecdir}
install -m 755 GNOME_MacMenuApplet.server %{buildroot}%{_libdir}/bonobo/servers

%clean
rm -rf %{buildroot}

%files
%defattr(-, root,root)
%{_libdir}/bonobo/servers/GNOME_MacMenuApplet.server
%{_libexecdir}/gnome-macmenu-applet

%changelog
* Sat Dec 22 2007 Yu Feng <rainwoodman@gmail.com>
- updated patch, avoid a core dump when quiting.
* Sat Dec 15 2007 Yu Feng <rainwoodman@gmail.com>
- new patch, added scrolling, merged the old 'previous window' patch.
* Fri Dec 7 2007 Yu Feng <rainwoodman@gmail.com>
- initial SPEC file
