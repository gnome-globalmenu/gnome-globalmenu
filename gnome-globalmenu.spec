%global base_version 0.7.6
%global pkgdocdir %{_docdir}/%{name}-%{version}
#%define alphatag 20080418svn2511

Name:		gnome-globalmenu
Version:	%{base_version}
Release:	3%{?dist}
Summary:	Global Menu for GNOME
Group:		User Interface/Desktops
License:	GPLv2 and LGPLv2
URL:		http://code.google.com/p/gnome2-globalmenu/
Source0:	http://gnome2-globalmenu.googlecode.com/files/gnome-globalmenu-%{base_version}.tar.bz2
BuildRoot:	%(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXXX)
BuildRequires: libwnck-devel
BuildRequires: intltool
BuildRequires: libXres-devel
BuildRequires: gnome-panel-devel
BuildRequires: libnotify-devel
BuildRequires: gnome-menus-devel

%if 0%{?fedora}
BuildRequires: xfce4-panel-devel
%endif

%description
GNOME Global Menu is a centralized menu bar for all windows on a particular 
screen/session. This package extends GTK and gnome panel so that Global Menu 
can be enabled on all GTK applications.
The Gtk Plugin Module of Global Menu adds global menu feature to any GTK 
applications on the fly without the need of modifying the source code.
gnome-applet-globalmenu or xfce-globalmenu-plugin should also be installed
depending on the desktop environment.

%package		common
Summary:		Shared configurations and translations of Global Menu packages
Group:			User Interface/Desktops
%description	common
GNOME Global Menu is a centralized menu bar for all windows on a particular 
screen/session. The Gtk Plugin Module of Global Menu adds global menu feature to
any GTK applications on the fly without the need of modifying the source code.
This package contains shared data and libraries of various Global Menu 
packages. gnome-applet-globalmenu or xfce-globalmenu-plugin should also be 
installed depending on the desktop environment.

%package		devel
Summary:		Header files for writing Global Menu applets
Group:			User Interface/Desktops
Requires:		pkgconfig
Requires:		gnome-globalmenu-common = %{version}-%{release}
%description	devel
This package contains header files for writing Global Menu applet in Gtk.

%package -n 	gnome-applet-globalmenu
Summary:		GNOME panel applet of Global Menu
Group:			User Interface/Desktops
Requires:		gnome-panel
Requires:		gnome-globalmenu-common = %{version}-%{release}
Provides:		gnome-globalmenu = %{version}-%{release}
Requires(pre): GConf2
Requires(post): GConf2
Requires(preun): GConf2
%description -n	gnome-applet-globalmenu
The GNOME panel applet of Global Menu is a representation of Global Menu 
with GTK widgets. The applet can be inserted to the default top panel to 
provide access to the Global Menu of the applications. 
The applet also provides limited window management functionalities. 
Please refer to /usr/share/doc/gnome-globalmenu-%{version}/README.GNOME 
for post-installation configurations.

%if 0%{?fedora}
%package -n		xfce4-globalmenu-plugin
Summary:		XFCE panel applet of Global Menu
Group:			User Interface/Desktops
Requires:		xfce4-panel
Requires:		gnome-globalmenu-common = %{version}-%{release}
%description -n	xfce4-globalmenu-plugin
The XFCE panel applet of Global Menu is a representation of Global Menu 
with GTK widgets. The applet can be inserted to the default top panel 
to provide access to the Global Menu of the applications. 
Please refer to /usr/share/doc/gnome-globalmenu-%{version}/README.XFCE 
for post installation configuations.
%endif

%prep
%setup -q -n %{name}-%{base_version}


%build
%configure --disable-schemas-install \
		--disable-static \
		--disable-tests \
		--with-gnome-panel \
		--docdir=%{pkgdocdir} \
%if 0%{?fedora}
		--with-xfce4-panel
%else
		--without-xfce4-panel
%endif
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT INSTALL="install -p"
%find_lang %{name}
rm -f $RPM_BUILD_ROOT/%{_libdir}/gtk-2.0/modules/libglobalmenu-gnome.la
rm -f $RPM_BUILD_ROOT/%{_libdir}/libgnomenu.la
rm -f $RPM_BUILD_ROOT/%{pkgdocdir}/INSTALL

%clean
rm -rf $RPM_BUILD_ROOT

%pre -n gnome-applet-globalmenu
if [ "$1" -gt 1 ] ; then
	export GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source`
	gconftool-2 --makefile-uninstall-rule \
	%{_sysconfdir}/gconf/schemas/%{name}.schemas >/dev/null || :
fi

%post -n gnome-applet-globalmenu
export GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source`
gconftool-2 --makefile-install-rule \
		%{_sysconfdir}/gconf/schemas/%{name}.schemas > /dev/null || :

%preun -n gnome-applet-globalmenu
if [ "$1" -eq 0 ] ; then
export GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source`
gconftool-2 --makefile-uninstall-rule \
		%{_sysconfdir}/gconf/schemas/%{name}.schemas > /dev/null || :
fi

%post common -p /sbin/ldconfig

%postun common -p /sbin/ldconfig

%files common -f %{name}.lang
%defattr(-,root,root,-)
%doc %{pkgdocdir}/*
%{_datadir}/pixmaps/globalmenu.png
%{_libdir}/libgnomenu-%{base_version}.so.2
%{_libdir}/libgnomenu-%{base_version}.so.2.0.0
%{_mandir}/man1/gnome-globalmenu.1.gz
%{_libdir}/gtk-2.0/modules/libglobalmenu-gnome.so
%{_libdir}/gtk-2.0/modules/libglobalmenu-gnome-%{base_version}.so

%files devel
%defattr(-,root,root,-)
%{_includedir}/libgnomenu/*.h
%{_libdir}/pkgconfig/libgnomenu.pc
%{_libdir}/libgnomenu.so
%dir %{_includedir}/libgnomenu

%files -n gnome-applet-globalmenu
%defattr(-,root,root,-)
%{_libdir}/bonobo/servers/GlobalMenu_PanelApplet.server
%{_libexecdir}/GlobalMenu.PanelApplet
%{_sysconfdir}/gconf/schemas/gnome-globalmenu.schemas

%if 0%{?fedora}
%files -n xfce4-globalmenu-plugin
%defattr(-,root,root,-)
%{_datadir}/xfce4/panel-plugins/GlobalMenu_XFCEPlugin.desktop
%{_libexecdir}/xfce4/panel-plugins/GlobalMenu.XFCEPlugin
%{_datadir}/pixmaps/globalmenu-xfce.png
%endif

%changelog
* Mon May 25 2009 Christoph Wickert <cwickert@fedorakrojekt.org> - 0.7.5-3
- Fix conditionals
- Rename base package to -common
- Let gnome-applet-globalmenu provide gnome-globalmenu for 
- Move GConf schema to gnome-applet package
- Fix script dependencies

* Tue Apr 18 2009 Feng Yu <rainwoodman@gmail.com> - 0.7.5-2
- Install documentation into the correct location

* Tue Apr 18 2009 Feng Yu <rainwoodman@gmail.com> - 0.7.5-1
- Update to 0.7.5

* Tue Apr 18 2009 Feng Yu <rainwoodman@gmail.com> - 0.7.5-0.1.20080418svn2507
- Bump to pre0.7.5

* Tue Apr 7 2009 Feng Yu <rainwoodman@gmail.com> - 0.7.4-4.20080407svn2489
- Bump to a pre0.7.5 svn snapshot.
- Replace rhel5 with rhel (suggested by Christoph Wickert)
- Added README.XFCE README.GNOME COPYING
- Correct the license
- initial support of Alt-<Key>s

* Sun Mar 8 2009 Feng Yu <rainwoodman@gmail.com> - 0.7.4-4
- Remove the -common package, merge it to the main package.

* Sun Mar 8 2009 Feng Yu <rainwoodman@gmail.com> - 0.7.4-3
- Rename the applet/plugin subpackages according to https://bugzilla.redhat.com/show_bug.cgi?id=480279#c14

* Sun Mar 8 2009 Feng Yu <rainwoodman@gmail.com> - 0.7.4-2
- Changes according to https://bugzilla.redhat.com/show_bug.cgi?id=480279#c14
- Use *.h and _% dir in devel package.
- Requires pkgconfig in devel package.
- Added versions to sub-package requirements.
- Removed gnome-menus and gtk2 from requires
- Moved GConf2 requires to -common.
- Added ldconfig requires to -common.
- Merge gtkmodule to -common.
- Moved libgnomenu.so to -devel
- Added doc tag in common

* Sun Mar 8 2009 Feng Yu <rainwoodman@gmail.com>
- Update to version 0.7.4.
- Generate the versioned .spec file from .spec.in with configure

* Thu Feb 14 2009 Feng Yu <rainwoodman@gmail.com>
- Valentine's day. 
- Update to version (post) 0.7.3.
- Generate the versioned .spec file from .spec.in with configure
- Build on rhel5.

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



