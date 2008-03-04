%define base_version 0.4
%define svn_version svn648
Name: 		gnome-globalmenu
Version: 	%{base_version}.%{svn_version}
Release:	2%{?dist}
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
Summary: libgnomenu Provides global menu widget GnomenuMenuBar
Group:		User Interface/Desktops
Requires: gtk2
%description -n libgnomenu

%package -n libgnomenu-devel
Summary: libgnomenu Provides global menu widget GnomenuMenuBar
Group:		User Interface/Desktops
Requires: gtk2
%description -n libgnomenu-devel

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
/usr/lib/libgnomenu.a
/usr/lib/libgnomenu.la
/usr/lib/libgnomenu.so
/usr/lib/libgnomenu.so.0
/usr/lib/libgnomenu.so.0.0.0
/usr/share/doc/gnome-globalmenu/AUTHORS
/usr/share/doc/gnome-globalmenu/COPYING
/usr/share/doc/gnome-globalmenu/ChangeLog
/usr/share/doc/gnome-globalmenu/INSTALL
/usr/share/doc/gnome-globalmenu/NEWS

%files -n libgnomenu-devel
%defattr(-, root, root)
/usr/include/libgnomenu/clienthelper.h
/usr/include/libgnomenu/messages.h
/usr/include/libgnomenu/quirks.h
/usr/include/libgnomenu/serverhelper.h
/usr/include/libgnomenu/menubar.h
/usr/include/libgnomenu/socket.h
/usr/lib/pkgconfig/libgnomenu.pc
/usr/share/doc/gnome-globalmenu/README

%files -n gnomenu-server
%defattr(-, root, root)
/usr/libexec/globalmenu-server

%files -n gnome-globalmenu-applet
%defattr(-, root, root)
/usr/libexec/gnome-globalmenu-applet
/usr/lib/bonobo/servers/GNOME_GlobalMenuApplet.server
/etc/gconf/schemas/gnome-globalmenu-applet.schemas

%files -n xfce-globalmenu-plugin
%defattr(-, root, root)
/usr/libexec/xfce-globalmenu-plugin
/usr/share/xfce4/panel-plugins/xfce-globalmenu-plugin.desktop

%changelog 
* Fri Feb 29 2008 Feng Yu <rainwoodman@gmail.com>
- Split into many sub packages.
* Mon Jan 14 2008 Feng Yu <rainwoodman@gmail.com>
- Added schema
* Wed Jan 09 2008 Feng Yu <rainwoodman@gmail.com>
- Added description
