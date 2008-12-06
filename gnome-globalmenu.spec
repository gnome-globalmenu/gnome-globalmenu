%define 	base_version 	0.6.9
%define 	svn_version 	svn1699
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

%description
GNOME Global Menu project aims to improve GNOME toward a Document Centric Desktop Environment. Global Menu is a menu bar shared with every window in this screen/session. This package extends GTK and gnome panel in a way such that Global Menu can be enabled on all GTK applications.

%prep
%setup -q -n %{name}-%{base_version}


%build
%configure
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT


%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root,-)

%{_libdir}/bonobo/servers/GlobalMenu_PanelApplet.server
%{_libdir}/libglobalmenu-gnome-0.7.0.so.1.0.0
%{_libdir}/libglobalmenu-gnome-0.7.0.so.1
%{_libdir}/libglobalmenu-gnome.a
%{_libdir}/libglobalmenu-gnome.la
%{_libdir}/libglobalmenu-gnome.so
%{_libexecdir}/GlobalMenu.PanelApplet


%changelog
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

