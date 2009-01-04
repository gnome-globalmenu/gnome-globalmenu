%define 	base_version 	0.7.2
%define 	svn_version 	svn1891
Name:		gnome-globalmenu
Version:	%{base_version}.%{svn_version}
Release:	1%{?dist}
Summary:	Global Menu for GNOME

Group:		User Interface/Desktops
License:	GPLv2+
URL:		http://code.google.com/p/gnome2-globalmenu/
Source0:		http://gnome2-globalmenu.googlecode.com/files/gnome-globalmenu-%{base_version}.tar.gz
BuildRoot:	%(mktemp -ud %{_tmppath}/%{name}-%{base_version}-%{release}-XXXXXXX)

Requires:		gtk2
Requires:		libwnck
Requires:		gnome-panel

Requires(pre): GConf2
Requires(post): GConf2
Requires(preun): GConf2


%description
GNOME Global Menu project aims to improve GNOME toward a Document Centric Desktop Environment. Global Menu is a menu bar shared with every window in this screen/session. This package extends GTK and gnome panel in a way such that Global Menu can be enabled on all GTK applications.

%prep
%setup -q -n %{name}-%{base_version}


%build
%configure --disable-schemas-install --disable-static --disable-tests
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
%find_lang %{name}
rm -f $RPM_BUILD_ROOT/%{_libdir}/gtk-2.0/modules/libglobalmenu-gnome.la

%clean
rm -rf $RPM_BUILD_ROOT

%pre
if [ "$1" -gt 1 ] ; then
	export GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source`
	gconftool-2 --makefile-uninstall-rule \
	%{_sysconfdir}/gconf/schemas/gnome-globalmenu.schemas >/dev/null || :
fi

%post
export GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source`
gconftool-2 --makefile-install-rule \
		%{_sysconfdir}/gconf/schemas/gnome-globalmenu.schemas > /dev/null || :

%preun
if [ "$1" -eq 0 ] ; then
export GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source`
gconftool-2 --makefile-uninstall-rule \
		%{_sysconfdir}/gconf/schemas/gnome-globalmenu.schemas > /dev/null || :
		fi

%files -f %{name}.lang
%defattr(-,root,root,-)
%{_libdir}/bonobo/servers/GlobalMenu_PanelApplet.server
%{_libdir}/gtk-2.0/modules/libglobalmenu-gnome.so
%{_libdir}/gtk-2.0/modules/libglobalmenu-gnome-0.7.0.so
%{_libexecdir}/GlobalMenu.PanelApplet
%{_sysconfdir}/gconf/schemas/gnome-globalmenu.schemas
%{_datadir}/pixmaps/globalmenu.png




%changelog
* Sun Dec 21 2008 Feng Yu <rainwoodman@gmail.com>
- 0.7 release.
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

