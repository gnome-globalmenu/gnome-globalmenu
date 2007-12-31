Name: 		gnome2-globalmenu-applet
Version: 	0.3
Release:	1%{?dist}
Summary:	Global menubar applet for Gnome2

Group:		User Interface/Desktops
License:	GPLv2+
URL:		http://code.google.com/p/gnome2-globalmenu/

Source0:	http://gnome2-globalmenu.googlecode.com/files/gnome2-globalmenu-applet-%{version}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

Requires:	gtk2-aqd
Requires:	libwnck

%description
Null

%prep
%setup

%build
%configure
make
cat GNOME_GlobalMenuApplet.server.sample | sed "s;APP_LOCATION;"%{_libexecdir}";" > GNOME_GlobalMenuApplet.server

%install
rm -rf %{buildroot}
make DESTDIR=%{buildroot} install-exec
install -d %{buildroot}%{_libdir}/bonobo/servers
install -p GNOME_GlobalMenuApplet.server %{buildroot}%{_libdir}/bonobo/servers

%clean
rm -rf %{buildroot}

%files
%defattr(-, root, root)
#%doc %{_docdir}/%{name}/README 
#%doc %{_docdir}/AUTHORS
#%doc %{_docdir}/COPYING 
#%doc %{_docdir}/ChangeLog
#%doc %{_docdir}/INSTALL 
#%doc %{_docdir}/NEWS
#%doc %{_docdir}/GNOME_GlobalMenuApplet.server.sample
%doc README AUTHORS COPYING ChangeLog INSTALL NEWS
%doc GNOME_GlobalMenuApplet.server.sample
%{_libexecdir}/gnome2-globalmenu-applet
%{_libdir}/bonobo/servers/GNOME_GlobalMenuApplet.server

