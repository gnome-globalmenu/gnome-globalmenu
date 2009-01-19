%define 	base_version 	0.7.2
%define 	svn_version 	svn2117
Name:		gnome-globalmenu
Version:	%{base_version}.%{svn_version}
Release:	2%{?dist}
Summary:	Global Menu for GNOME

Group:		User Interface/Desktops
License:	GPLv2+
URL:		http://code.google.com/p/gnome2-globalmenu/
Source0:		http://gnome2-globalmenu.googlecode.com/files/gnome-globalmenu-%{base_version}.tar.gz
BuildRoot:	%(mktemp -ud %{_tmppath}/%{name}-%{base_version}-%{release}-XXXXXXX)

Requires(pre): GConf2
Requires(post): GConf2
Requires(preun): GConf2

%description
GNOME Global Menu project aims to improve GNOME toward a Document Centric Desktop Environment. Global Menu is a menu bar shared with every window in this screen/session. This package extends GTK and gnome panel in a way such that Global Menu can be enabled on all GTK applications.

%package		common
Summary:		Shared configurations and translations of Global Menu packages
Group:			User Interface/Desktops
%description	common
This package contains shared configurations and translations of various Global Menu packages.

%package 		gtkmodule
Summary:		Gtk Plugin Module of Global Menu
Group: 			User Interface/Desktops
Requires:		gtk2
Requires:		gnome-globalmenu-common
%description	gtkmodule
The Gtk Plugin Module of Global Menu adds global menu feature to any GTK applications on the fly without the need of modifying the source code.

%package		gnome-panel
Summary:		GNOME panel applet of Global Menu
Group:			User Interface/Desktops
Requires:		gtk2
Requires:		gnome-panel
Requires:		libwnck
Requires:		libnotify
Requires:		gnome-menus
Requires:		gnome-globalmenu-common
%description 	gnome-panel
The GNOME panel applet of Global Menu is a representation of Global Menu with GTK widgets. The applet can be inserted to the default top panel to provide access to the Global Menu of the applications. 
The applet also provides limited window management functionalities.

%package		xfce-panel
Summary:		XFCE panel applet of Global Menu
Group:			User Interface/Desktops
Requires:		gtk2
Requires:		xfce4-panel
Requires:		libwnck
Requires:		gnome-globalmenu-common
%description 	xfce-panel
The XFCE panel applet of Global Menu is a representation of Global Menu with GTK widgets. The applet can be inserted to the default top panel to provide access to the Global Menu of the applications. 

%prep
%setup -q -n %{name}-%{base_version}


%build
%configure --disable-schemas-install --disable-static --disable-tests --with-gnome-panel --with-xfce4-panel
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
%find_lang %{name}
rm -f $RPM_BUILD_ROOT/%{_libdir}/gtk-2.0/modules/libglobalmenu-gnome.la

%clean
rm -rf $RPM_BUILD_ROOT

%pre common
if [ "$1" -gt 1 ] ; then
	export GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source`
	gconftool-2 --makefile-uninstall-rule \
	%{_sysconfdir}/gconf/schemas/gnome-globalmenu.schemas >/dev/null || :
fi

%post common
export GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source`
gconftool-2 --makefile-install-rule \
		%{_sysconfdir}/gconf/schemas/gnome-globalmenu.schemas > /dev/null || :

%preun common
if [ "$1" -eq 0 ] ; then
export GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source`
gconftool-2 --makefile-uninstall-rule \
		%{_sysconfdir}/gconf/schemas/gnome-globalmenu.schemas > /dev/null || :
		fi

%files common -f %{name}.lang
%defattr(-,root,root,-)
%{_sysconfdir}/gconf/schemas/gnome-globalmenu.schemas
%{_datadir}/pixmaps/globalmenu.png

%files gnome-panel
%{_libdir}/bonobo/servers/GlobalMenu_PanelApplet.server
%{_libexecdir}/GlobalMenu.PanelApplet
%files xfce-panel
%{_datadir}/xfce4/panel-plugins/GlobalMenu_XFCEPlugin.desktop
%{_libexecdir}/xfce4/panel-plugins/GlobalMenu.XFCEPlugin
%{_datadir}/pixmaps/globalmenu-xfce.png

%files gtkmodule
%{_libdir}/gtk-2.0/modules/libglobalmenu-gnome.so
%{_libdir}/gtk-2.0/modules/libglobalmenu-gnome-%{base_version}.so

%changelog
* Thu Jan 8 2009 Feng Yu <rainwoodman@gmail.com>
- Add XFCE4 plugin
* Thu Jan 8 2009 Feng Yu <rainwoodman@gmail.com>
- Spawn into sub packages.
* Wed Dec 17 2008 Feng Yu <rainwoodman@gmail.com>
- The module is ready for loaded/unloaded by GtkSettings. Because an issue with scim-bridge, the gconf key keeps disabling the module by default.
* Tue Dec 15 2008 Feng Yu <rainwoodman@gmail.com>
- gnome-settings-daemon for the module. (gconf-key: apps/gnome-settings-daemon/gtk-modules)
* Tue Dec 15 2008 Feng Yu <rainwoodman@gmail.com>
- vala 0.5.2
* Sun Dec 14 2008 Feng Yu <rainwoodman@gmail.com>
- GConf for applet 
* Sun Dec 10 2008 Feng Yu <rainwoodman@gmail.com>
- RGBA support for popup menus
* Sun Dec 5 2008 Feng Yu <rainwoodman@gmail.com>
- update to 0.7
- use the default vim template for spec files.
* Sun Oct 7 2008 Feng Yu <rainwoodman@gmail.com>
- installing libglobalmenu-gnome to gtk-2.0/modules
* Sun Oct 7 2008 Feng Yu <rainwoodman@gmail.com>
- Update to svn 1351
- Divide into sub packages
* Sun Oct 5 2008 Feng Yu <rainwoodman@gmail.com>
- Update to 0.6
* Sun Mar 23 2008 Feng Yu <rainwoodman@gmail.com>
- change to macros for building x86-64 rpms
* Fri Mar 14 2008 Feng Yu <rainwoodman@gmail.com>
- remove language pack
- move shared files to gnome-globalmenu
* Wed Mar 12 2008 Feng Yu <rainwoodman@gmail.com>
- Separated .gmo files
- Added description for packages.
* Sun Mar 9 2008 Feng Yu <rainwoodman@gmail.com>
- Properly install doc.
- Added French locale 
* Fri Mar 7 2008 Feng Yu <rainwoodman@gmail.com>
- Install doc
- Added depencency
- Added the mo file.
- distribute gtk2-aqd patch
* Fri Mar 5 2008 Feng Yu <rainwoodman@gmail.com>
- Enable schemas.
* Fri Feb 29 2008 Feng Yu <rainwoodman@gmail.com>
- Split into many sub packages.
* Mon Jan 14 2008 Feng Yu <rainwoodman@gmail.com>
- Added schema
* Wed Jan 09 2008 Feng Yu <rainwoodman@gmail.com>
- Added description

# vim:ts=4:sw=4

