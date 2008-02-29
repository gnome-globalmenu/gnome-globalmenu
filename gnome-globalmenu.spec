%define base_version 0.4
%define svn_version svn556
Name: 		gnome-globalmenu
Version: 	%{base_version}.%{svn_version}
Release:	1%{?dist}
Summary:	Global menu bar widget and library for GTK/GNOME2

Group:		User Interface/Desktops
License:	GPLv2+
URL:		http://code.google.com/p/gnome2-globalmenu/

Source0:	http://gnome2-globalmenu.googlecode.com/files/gnome-globalmenu-%{base_version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{base_version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:	gtk2
BuildRequires:	libwnck
BuildRequires:	gnome-panel-devel
BuildRequires:	bonobo-activation
BuildRequires:	xfce4-panel-devel
%description


%package -n libgnomenu
Summary: libgnomenu Provides global menu widget GtkGlobalMenuBar 
Group:		User Interface/Desktops
Requires: gtk2
%description -n libgnomenu

%package -n gnome-globalmenu-applet
Requires: libgnomenu
Summary: gnome-panel applet for global menu.
Group:		User Interface/Desktops
%description -n gnome-globalmenu-applet

%package -n xfce-globalmenu-plugin
Summary: xfce-panel plugin for global menu.
Requires: libgnomenu
Group:		User Interface/Desktops
%description -n xfce-globalmenu-plugin

%package -n gnomenu-server
Summary: standalone mene server for global menu.
Requires: libgnomenu
Group:		User Interface/Desktops
%description -n gnomenu-server

%prep
%setup -q -n gnome-globalmenu-%{base_version}

%build
%configure
make

%install
rm -rf %{buildroot}
make DESTDIR=%{buildroot} install

%clean
rm -rf %{buildroot}

%pre
#export GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source`
#gconftool-2 --makefile-uninstall-rule %{_sysconfdir}/gconf/schemas/gnome2-globalmenu-applet.schema  >& /dev/null || :

%post
#export GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source`
#gconftool-2 --makefile-install-rule %{_sysconfdir}/gconf/schemas/gnome2-globalmenu-applet.schema >& /dev/null || :

%preun
#export GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source`
#gconftool-2 --makefile-uninstall-rule %{_sysconfdir}/gconf/schemas/gnome2-globalmenu-applet.schema >& /dev/null || :


%files -n libgnomenu
%defattr(-, root, root)
/etc/libgnomenu.conf
/usr/lib/gtk-2.0/modules/libgnomenu.a
/usr/lib/gtk-2.0/modules/libgnomenu.la
/usr/lib/gtk-2.0/modules/libgnomenu.so
/usr/lib/gtk-2.0/modules/libgnomenu.so.0
/usr/lib/gtk-2.0/modules/libgnomenu.so.0.0.0
/usr/include/libgnomenu/clienthelper.h
/usr/include/libgnomenu/messages.h
/usr/include/libgnomenu/quirks.h
/usr/include/libgnomenu/serverhelper.h
/usr/include/libgnomenu/menubar.h
/usr/include/libgnomenu/socket.h
/usr/lib/pkgconfig/libgnomenu.pc
/usr/share/doc/gnome-globalmenu/AUTHORS
/usr/share/doc/gnome-globalmenu/COPYING
/usr/share/doc/gnome-globalmenu/ChangeLog
/usr/share/doc/gnome-globalmenu/INSTALL
/usr/share/doc/gnome-globalmenu/NEWS
/usr/share/doc/gnome-globalmenu/README

%files -n gnomenu-server
/usr/libexec/globalmenu-server

%files -n gnome-globalmenu-applet
/usr/libexec/gnome-globalmenu-applet
/usr/lib/bonobo/servers/GNOME_GlobalMenuApplet.server

%files -n xfce-globalmenu-plugin
/usr/libexec/xfce-globalmenu-plugin
/usr/share/xfce4/panel-plugins/xfce-globalmenu-plugin.desktop

%changelog 
* Mon Jan 14 2008 Feng Yu <rainwoodman@gmail.com>
- Added schema
* Wed Jan 09 2008 Feng Yu <rainwoodman@gmail.com>
- Added description
