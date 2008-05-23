# Note that this is NOT a relocatable package

%define glib2_base_version 2.13.7
%define glib2_version %{glib2_base_version}-1
%define pango_base_version 1.17.3
%define pango_version %{pango_base_version}-1
%define atk_base_version 1.9.0
%define atk_version %{atk_base_version}-1
%define cairo_base_version 1.2.0
%define cairo_version %{cairo_base_version}-1
%define libpng_version 2:1.2.2-16

%define base_version 2.12.9
%define bin_version 2.10.0
%define svn_version 0.4.svn987

Summary: The GIMP ToolKit (GTK+), a library for creating GUIs for X
Name: gtk2-aqd
Version: %{svn_version}.%{base_version}
Release: 5%{?dist}
License: LGPLv2+
Group: System Environment/Libraries
Source: http://download.gnome.org/sources/gtk+/2.12/gtk+-%{base_version}.tar.bz2
Source1: update-gdk-pixbuf-loaders
Source2: update-gtk-immodules 

Provides: gtk2 = %{base_version}-%{release}
Obsoletes: gtk2 <= %{base_version}-%{release}
# Biarch changes
Patch0: gtk+-2.4.1-lib64.patch
# Fedora patch
Patch1: gtk+-2.11.1-set-invisible-char-to-bullet.patch
# a workaround for some brokenness in the flash plugin
# see http://bugzilla.gnome.org/show_bug.cgi?id=463773
Patch2: workaround.patch

# http://bugzilla.gnome.org/show_bug.cgi?id=488119
Patch3: system-log-crash.patch

# backport from svn trunk
Patch4: im-setting.patch

# fixed upstream
Patch5: foreign-cmap.patch

# fixed upstream
Patch6: implicit-g_fopen.patch

# fixed upstream
Patch7: filechooser-iconsize.patch

# http://bugzilla.gnome.org/show_bug.cgi?id=521032
Patch8: filechooser-auth.patch

# http://bugzilla.gnome.org/show_bug.cgi?id=467698
Patch9: tab-drag-crash.patch

# gtk-aqd patch
Patch998: gtk2-menubar-overflow.patch 
Patch999: gtk2-aqd.patch 

BuildRequires: atk-devel >= %{atk_version}
BuildRequires: pango-devel >= %{pango_version}
BuildRequires: glib2-devel >= %{glib2_version}
BuildRequires: libtiff-devel
BuildRequires: libjpeg-devel
BuildRequires: libXi-devel
BuildRequires: libpng-devel >= %{libpng_version}
BuildRequires: automake17 autoconf libtool pkgconfig
BuildRequires: gettext
BuildRequires: gtk-doc
BuildRequires: cups-devel
BuildRequires: cairo-devel >= %{cairo_version}
BuildRequires: libXrandr-devel
BuildRequires: libXrender-devel
BuildRequires: libXcursor-devel
BuildRequires: libXfixes-devel
BuildRequires: libXinerama-devel
BuildRequires: libXcomposite-devel

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

# Conflicts with packages containing theme engines
# built against the 2.4.0 ABI
Conflicts: gtk2-engines < 2.7.4-7
Conflicts: libgnomeui < 2.15.1cvs20060505-2
Conflicts: redhat-artwork < 0.243-1

URL: http://www.gtk.org

# required for icon themes apis to work
Requires: hicolor-icon-theme

# We need to prereq these so we can run gtk-query-immodules-2.0
Requires(post): glib2 >= %{glib2_version}
Requires(post): atk >= %{atk_version}
Requires(post): pango >= %{pango_version}
# and these for gdk-pixbuf-query-loaders
Requires(post): libtiff >= 3.6.1

%define _unpackaged_files_terminate_build      1
%define _missing_doc_files_terminate_build     1


%description
GTK+ is a multi-platform toolkit for creating graphical user
interfaces. Offering a complete set of widgets, GTK+ is suitable for
projects ranging from small one-off tools to complete application
suites.

%package devel
Summary: Development tools for GTK+ applications
Group: Development/Libraries
Requires: gtk2-aqd = %{version}-%{release}
Requires: pango-devel >= %{pango_version}
Requires: atk-devel >= %{atk_version}
Requires: glib2-devel >= %{glib2_version}
Requires: cairo-devel >= %{cairo_version}
Requires: libX11-devel, libXcursor-devel, libXinerama-devel
Requires: libXext-devel, libXi-devel, libXrandr-devel
Requires: libXfixes-devel, libXcomposite-devel
Requires: libpng-devel
Requires: pkgconfig
# for /usr/share/aclocal
Requires: automake
# for /usr/share/gtk-doc/html
Requires: gtk-doc
Provides: gtk2-devel = %{base_version}-%{release}
Obsoletes: gtk2-devel <= %{base_version}-%{release}
%description devel
The gtk+-devel package contains the header files and developer
docs for the GTK+ widget toolkit.  

%prep
%setup -q -n gtk+-%{base_version}

%patch0 -p1 -b .lib64
%patch1 -p1 -b .set-invisible-char-to-bullet
%patch2 -p1 -b .workaround
%patch3 -p1 -b .system-log-crash
%patch4 -p1 -b .im-setting
%patch5 -p1 -b .foreign-cmap
%patch6 -p1 -b .implicit-g_fopen
%patch7 -p1 -b .filechooser-iconsize
%patch8 -p1 -b .filechooser-auth
%patch9 -p1 -b .tab-drag-crash

#%patch998 -p1 -b .menubar-overflow
%patch999 -F3 -p1 -b .aqd
for i in config.guess config.sub ; do
  test -f %{_datadir}/libtool/$i && cp %{_datadir}/libtool/$i .
done

%build
libtoolize --force

# Patch0 modifies gdk-pixbuf/Makefile.am
aclocal-1.7
automake-1.7
autoconf
autoheader

if ! pkg-config --exists pangoxft ; then
        echo "No pangoxft.pc!"
        exit 1
fi

%configure --with-xinput=xfree --disable-gtk-doc --disable-rebuilds --with-included-loaders=png 

## smp_mflags doesn't work for now due to gdk-pixbuf.loaders, may be fixed 
## past gtk 2.1.2
make CFLAGS="-ggdb -O0" ## %{?_smp_mflags}
# turn off for now, since floatingtest needs a display
#make check

# create a dummy binary for /usr/lib/gtk-2.0/immodules to work around
# problems in our ia64 multilib infrastructure
# See https://bugzilla.redhat.com/show_bug.cgi?id=253726 for more details
echo 'int main (void) { return 0; }' > relocation-tag.c
gcc -Os relocation-tag.c -o .relocation-tag

%install
rm -rf $RPM_BUILD_ROOT
# Deriving /etc/gtk-2.0/$host location
# NOTE: Duplicated below
#
# autoconf changes linux to linux-gnu
case "%{_host}" in
  *linux) host="%{_host}-gnu"
  ;;
  *) host="%{_host}"
  ;;
esac

# autoconf uses powerpc not ppc
host=`echo $host | sed "s/^ppc/powerpc/"`

# Make sure that the host value that is passed to the compile 
# is the same as the host that we're using in the spec file
#
compile_host=`grep 'host_triplet =' gtk/Makefile | sed "s/.* = //"`

if test "x$compile_host" != "x$host" ; then
  echo 1>&2 "Host mismatch: compile='$compile_host', spec file='$host'" && exit 1
fi

make install DESTDIR=$RPM_BUILD_ROOT        \
             RUN_QUERY_IMMODULES_TEST=false \
             RUN_QUERY_LOADER_TEST=false 

%find_lang gtk20
%find_lang gtk20-properties

cat gtk20.lang gtk20-properties.lang > all.lang

./mkinstalldirs $RPM_BUILD_ROOT%{_sysconfdir}/gtk-2.0
#
# Make cleaned-up versions of tutorials, examples, and faq for installation
#
./mkinstalldirs tmpdocs
cp -aR docs/tutorial/html tmpdocs/tutorial
cp -aR docs/faq/html tmpdocs/faq

for dir in examples/* ; do
  if [ -d $dir ] ; then
     ./mkinstalldirs tmpdocs/$dir
     for file in $dir/* ; do
       install -m 0644 $file tmpdocs/$dir
     done
  fi
done

# We need to have separate 32-bit and 64-bit binaries
# for places where we have two copies of the GTK+ package installed.
# (we might have x86_64 and i686 packages on the same system, for example.)
case "$host" in
  alpha*|ia64*|powerpc64*|s390x*|x86_64*)
   mv $RPM_BUILD_ROOT%{_bindir}/gtk-query-immodules-2.0 $RPM_BUILD_ROOT%{_bindir}/gtk-query-immodules-2.0-64
   mv $RPM_BUILD_ROOT%{_bindir}/gdk-pixbuf-query-loaders $RPM_BUILD_ROOT%{_bindir}/gdk-pixbuf-query-loaders-64
   ;;
  *)
   mv $RPM_BUILD_ROOT%{_bindir}/gtk-query-immodules-2.0 $RPM_BUILD_ROOT%{_bindir}/gtk-query-immodules-2.0-32
   mv $RPM_BUILD_ROOT%{_bindir}/gdk-pixbuf-query-loaders $RPM_BUILD_ROOT%{_bindir}/gdk-pixbuf-query-loaders-32
   ;;
esac

#
# Install wrappers for the binaries
#
cp %{SOURCE1} $RPM_BUILD_ROOT%{_bindir}/update-gdk-pixbuf-loaders
cp %{SOURCE2} $RPM_BUILD_ROOT%{_bindir}/update-gtk-immodules

# Remove unpackaged files
rm $RPM_BUILD_ROOT%{_libdir}/*.la
rm $RPM_BUILD_ROOT%{_libdir}/gtk-2.0/%{bin_version}/*/*.la

mkdir -p $RPM_BUILD_ROOT%{_sysconfdir}/gtk-2.0/$host
touch $RPM_BUILD_ROOT%{_sysconfdir}/gtk-2.0/$host/gtk.immodules
touch $RPM_BUILD_ROOT%{_sysconfdir}/gtk-2.0/$host/gdk-pixbuf.loaders

mkdir -p $RPM_BUILD_ROOT%{_libdir}/gtk-2.0/modules
mkdir -p $RPM_BUILD_ROOT%{_libdir}/gtk-2.0/immodules
mkdir -p $RPM_BUILD_ROOT%{_libdir}/gtk-2.0/%{bin_version}/filesystems

# we need to install a binary in the immodules directory to make sure
# that it gets properly relocated to /emul for ia64 emulation of x86
# See https://bugzilla.redhat.com/show_bug.cgi?id=253726
install -m 0644 .relocation-tag $RPM_BUILD_ROOT%{_libdir}/gtk-2.0/immodules

#
# We need the substitution of $host so we use an external
# file list
#
echo %dir %{_sysconfdir}/gtk-2.0/$host >> all.lang
echo %ghost %{_sysconfdir}/gtk-2.0/$host/gtk.immodules >> all.lang
echo %ghost %{_sysconfdir}/gtk-2.0/$host/gdk-pixbuf.loaders >> all.lang

%clean
rm -rf $RPM_BUILD_ROOT

%post
/sbin/ldconfig
/usr/bin/update-gdk-pixbuf-loaders %{_host}
/usr/bin/update-gtk-immodules %{_host}

%postun
/sbin/ldconfig

%files -f all.lang
%defattr(-, root, root)

%doc AUTHORS COPYING NEWS README
%{_bindir}/gdk-pixbuf-query-loaders*
%{_bindir}/gtk-query-immodules-2.0*
%{_bindir}/update-gdk-pixbuf-loaders
%{_bindir}/update-gtk-immodules
%{_bindir}/gtk-update-icon-cache
%{_libdir}/libgtk-x11-2.0.so.*
%{_libdir}/libgdk-x11-2.0.so.*
%{_libdir}/libgdk_pixbuf-2.0.so.*
%{_libdir}/libgdk_pixbuf_xlib-2.0.so.*
%dir %{_libdir}/gtk-2.0
%{_libdir}/gtk-2.0/%{bin_version}
%{_libdir}/gtk-2.0/immodules
%{_libdir}/gtk-2.0/modules
%{_datadir}/themes/Default
%{_datadir}/themes/Emacs
%{_datadir}/themes/Raleigh
%dir %{_sysconfdir}/gtk-2.0
%{_sysconfdir}/gtk-2.0/im-multipress.conf

%files devel
%defattr(-, root, root)

%{_libdir}/lib*.so
%{_libdir}/gtk-2.0/include
%{_datadir}/gtk-doc/html/*
%{_mandir}/man1/*
%{_includedir}/*
%{_datadir}/aclocal/*
%{_bindir}/gdk-pixbuf-csource
%{_bindir}/gtk-builder-convert
%{_libdir}/pkgconfig/*
%doc tmpdocs/tutorial
%doc tmpdocs/faq
%doc tmpdocs/examples
%{_bindir}/gtk-demo
%{_datadir}/gtk-2.0

%changelog
* Tue Jan  9 2008 Matthias Clasen  <mclasen@redhat.com> - 2.12.5-1
- Update to 2.12.5

* Tue Jan  8 2008 Matthias Clasen  <mclasen@redhat.com> - 2.12.4-1
- Update to 2.12.4
- Drop obsolete patches

* Wed Dec 19 2007 Colin Walters <walters@redhat.com> - 2.12.3-3
- BR libXcomposite-devel so we get the sexy.

* Tue Dec 11 2007 Matthias Clasen <mclasen@redhat.com> - 2.12.3-2
- Fix yet another notebook tab related crash

* Wed Dec  5 2007 Matthias Clasen <mclasen@redhat.com> - 2.12.3-1
- Update to 2.12.3

* Mon Nov 26 2007 Matthias Clasen <mclasen@redhat.com> - 2.12.2-1
- Update to 2.12.2

* Sun Nov  4 2007 Matthias Clasen <mclasen@redhat.com> - 2.12.1-6
- Include the /usr/lib/gtk-2.0/2.10.0/filesystems directory

* Thu Oct 25 2007 Matthias Clasen <mclasen@redhat.com> - 2.12.1-5
- Fix a bug that prevents GtkBuilder-using apps (like totem)
  to run in some locales (like Turkish) (#348631)

* Mon Oct 22 2007 Matthias Clasen <mclasen@redhat.com> - 2.12.1-4
- Fix a crash in gnome-system-log (#321701)

* Wed Oct 17 2007 Matthias Clasen <mclasen@redhat.com> - 2.12.1-2
- Fix a crash in the firefox print preview (#336771)

* Wed Oct 17 2007 Matthias Clasen <mclasen@redhat.com> - 2.12.1-1
- Update to 2.12.1 (bug fixes and translation updates)
- Drop obsolete patches

* Thu Oct 11 2007 Matthias Clasen <mclasen@redhat.com> - 2.12.0-6
- Fix a double-free problem in gtk-update-icon-cache (#327711)

* Thu Oct  4 2007 Matthias Clasen <mclasen@redhat.com> - 2.12.0-5
- Fix a grab problem with multiple volume buttons

* Tue Sep 25 2007 Matthias Clasen <mclasen@redhat.com> - 2.12.0-4
- Fix a crash in simple search
- Drop obsolete Obsoletes and Conflicts 

* Thu Sep 20 2007 Matthias Clasen <mclasen@redhat.com> - 2.12.0-3
- Fix a problem with swt and tooltips 

* Tue Sep 18 2007 Matthias Clasen <mclasen@redhat.com> - 2.12.0-2
- Adapt to tracker ABI changes

* Fri Sep 14 2007 Matthias Clasen <mclasen@redhat.com> - 2.12.0-1
- Update to 2.12.0

* Fri Sep  7 2007 Matthias Clasen <mclasen@redhat.com> - 2.11.6-9
- Add a workaround for the flash plugin

* Fri Sep  7 2007 Ray Strode <rstrode@redhat.com> - 2.11.6-8
- install dummy binary in libdir/gtk-2.0/immodules directory to
  aid rpm when doing ia64 multilib (bug 253726)

* Mon Aug 27 2007 Jens Petersen <petersen@redhat.com> - 2.11.6-7
- own libdir/gtk-2.0/immodules directory (#255621)

* Wed Aug  8 2007 Matthias Clasen <mclasen@redhat.com> - 2.11.6-6
- Improve tooltip compatibility to make acroread work again
 
* Sun Aug  5 2007 Matthias Clasen <mclasen@redhat.com> - 2.11.6-5
- Really move gtk-demo over

* Thu Aug  2 2007 Matthias Clasen <mclasen@redhat.com> - 2.11.6-4
- Move gtk-demo to the -devel package
- Don't install ChangeLog
- Update the License field

* Wed Jul 25 2007 Matthias Clasen <mclasen@redhat.com> - 2.11.6-3
- Fix the behaviour of tooltips on system tray icons

* Tue Jul 24 2007 Matthias Clasen <mclasen@redhat.com> - 2.11.6-2
- Silence the icon cache validator (#248789)

* Mon Jul 23 2007 Matthias Clasen <mclasen@redhat.com> - 2.11.6-1
- Update to 2.11.6
- Make it build against recent cups

* Thu Jul 19 2007 Matthias Clasen <mclasen@redhat.com> - 2.11.5-4
- Up the glib requirement

* Sun Jul  8 2007 Matthias Clasen <mclasen@redhat.com> - 2.11.5-3
- Own /usr/lib/gtk-2.0/modules

* Mon Jul  2 2007 Matthias Clasen <mclasen@redhat.com> - 2.11.5-1
- Update to 2.11.5

* Tue Jun 19 2007 Matthias Clasen <mclasen@redhat.com> - 2.11.4-1
- Update to 2.11.4

* Sun Jun 17 2007 Matthias Clasen <mclasen@redhat.com> - 2.11.3-4
- Update versioned dependencies (#244602)

* Sun Jun 17 2007 Matthias Clasen <mclasen@redhat.com> - 2.11.3-3
- Clean up directory ownership

* Sat Jun 16 2007 Caolan McNamara <caolanm@redhat.com> - 2.11.3-2
- Resolves: rhbz#244516 avoid typename in headers for C++

* Fri Jun 15 2007 Matthias Clasen <mclasen@redhat.com> - 2.11.3-1
- Update to 2.11.3
- Drop upstreamed patches

* Wed Jun  6 2007 Matthias Clasen <mclasen@redhat.com> - 2.11.2-1
- Update to 2.11.2

* Mon Jun  4 2007 Matthias Clasen <mclasen@redhat.com> - 2.11.1-1
- Update to 2.11.1
- Update patches

* Thu May 24 2007 Matthias Clasen <mclasen@redhat.com> - 2.11.0-1
- Update to 2.11.0
- Drop upstreamed patches

* Sat May 19 2007 Matthias Clasen <mclasen@redhat.com> - 2.10.12-1
- Update to 2.10.12
- Drop upstreamed patches

* Tue May 15 2007 Matthias Clasen <mclasen@redhat.com> - 2.10.11-6
- Backport some fixes for the ftw()-based search engine

* Tue Apr 10 2007 Matthias Clasen <mclasen@redhat.com> - 2.10.11-5
- Use DESKTOP xdg-user-dir in the file chooser

* Mon Apr  9 2007 Matthias Clasen <mclasen@redhat.com> - 2.10.11-4
- Fix a memory leak in the search patch

* Wed Mar 28 2007 Matthias Clasen <mclasen@redhat.com> - 2.10.11-3
- Support raw printers

* Tue Mar 20 2007 Florian La Roche <laroche@redhat.com> - 2.10.11-2
- fix Conflicts: libgnomeui line

* Wed Mar 14 2007 Matthias Clasen <mclasen@redhat.com> - 2.10.11-1
- Update to 2.10.11
- Require libpng-devel in the devel package (#232013)

* Mon Mar 12 2007 Matthias Clasen <mclasen@redhat.com> - 2.10.10-1
- Update to 2.10.10

* Fri Feb  9 2007 Stepan Kasal <skasal@redhat.com> - 2.10.9-4
- Clean up the autotools calls in %%prep.

* Fri Feb  2 2007 Matthias Clasen <mclasen@redhat.com> - 2.10.9-3
- Fix update-gtk-immodules and update-gdk-pixbuf-loaders 
  being swapped  (#227134)

* Tue Jan 30 2007 Matthias Clasen <mclasen@redhat.com> - 2.10.9-2
- Fix filechooser search support

* Mon Jan 22 2007 Matthias Clasen <mclasen@redhat.com> - 2.10.9-1
- Update to 2.10.9

* Wed Jan 17 2007 Matthias Clasen <mclasen@redhat.com> - 2.10.8-2
- Fix a crash in the recent-files menu code.

* Wed Jan 17 2007 Matthias Clasen <mclasen@redhat.com> - 2.10.8-1
- Update to 2.10.8

* Tue Jan 09 2007 Behdad Esfahbod <besfahbo@redhat.com> - 2.10.7-2
- Configure with --with-included-loaders=png.  Saves a page per process

* Thu Dec 21 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.7-1
- Make gdk_pixbuf_loader_close() idempotent
- Always emit the closed signal when the loader is closed

* Thu Dec 21 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.6-8
- Make update scripts handle slight variations in $host

* Sat Dec  9 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.6-7
- Fix error handling in pixbuf loaders (#218755)
- Fix clipping of mnemonic underlines (#218615)
- Give accessible names to message dialogs (#215472)
- Fix a crash in the handling of invalid icon themes (#218247)
- Make the print dialog work when the 'BrowseShortNames Off' cups
  option is used (#217220)

* Sat Nov 25 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.6-6
- Fix a recent-files related crash

* Tue Nov 21 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.6-5
- Change the search patch to check for beagle first

* Sun Nov 20 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.6-4
- Some spec file cleanups

* Fri Nov 17 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.6-3
- Rework the filechooser search to support tracker, too

* Thu Nov 16 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.6-2
- Avoid a possible segfault (#215933)

* Sat Sep 30 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.4-4
- Avoid a possible segfault (gnome #358405)

* Fri Sep 29 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.4-3
- Fix a possible deadlock when not using the gnome-vfs
  filesystem backend

* Sat Sep 23 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.4-2
- Fix a problem with the search patch

* Sat Sep 23 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.4-1
- Update to 2.10.4
- Drop upstreamed patches
- Update the search patch
- Require pkgconfig in the -devel package

* Tue Sep 19 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.3-7
- Fix issues with auth dialogs in the file chooser

* Wed Sep 13 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.3-6
- Don't spew a warning if libbeagle is not installed

* Wed Sep 13 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.3-5
- Make color picker work with window groups

* Sun Sep 10 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.3-4
- Fix display of Desktop in file chooser buttons.

* Fri Sep  8 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.3-3.fc6
- Fix a Sylpheed crash  (#192101)

* Mon Sep  5 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.3-2.fc6
- Use fam for recent files

* Tue Sep  5 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.3-1.fc6
- Update to 2.10.3

* Fri Sep  1 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.2-6.fc6
- Fix a problem with entering Hangul in entries

* Thu Aug 31 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.2-5.fc6
- Fix problems with listing printers
- Stop cursor blinking after a while, to save energy

* Mon Aug 28 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.2-4.fc6
- Use a reasonable timeout when polling for printer 
  list updates  (#203585)

* Wed Aug 23 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.2-3.fc6
- Fix confusion between values and names in printer options (#203588)

* Fri Aug 19 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.2-2.fc6
- Fix some problems with the recent files code

* Fri Aug 18 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.2-1.fc6
- Update to 2.10.2

* Mon Aug 14 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.1-3.fc6
- Fix a problem with the search patch

* Wed Aug  9 2006 Ray Strode <rstrode@redhat.com> - 2.10.1-2
- patch from Jonathan Matthew <jontahn@kaolin.wh9.net> to fix
  crash in GtkTreeModelFilter (upstream bug 346800)

* Sun Jul 23 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.1-1
- Update to 2.10.1

* Wed Jul 19 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.0-5
- Fix a typo in the Search support patch

* Tue Jul 18 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.0-4
- Make the Search support more bulletproof

* Sun Jul 16 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.0-3
- Fix a problem with the Search support

* Sat Jul 15 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.0-2
- Add Search support to the filechooser

* Wed Jul 12 2006 Jesse Keating <jkeating@redhat.com> - 2.10.0-1.1
- rebuild

* Mon Jul  3 2006 Matthias Clasen <mclasen@redhat.com> - 2.10.0-1
- Update to 2.10.0

* Wed Jun 21 2006 Matthias Clasen <mclasen@redhat.com> - 2.9.4-1
- Update to 2.9.4

* Thu Jun 15 2006 Matthias Clasen <mclasen@redhat.com> - 2.9.3-4
- Add more BuildRequires

* Wed Jun 14 2006 Matthias Clasen <mclasen@redhat.com> - 2.9.3-3
- Require cairo 1.1.8

* Wed Jun 14 2006 Tomas Mraz <tmraz@redhat.com> - 2.9.3-2
- rebuilt with new gnutls

* Tue Jun 13 2006 Matthias Clasen <mclasen@redhat.com> - 2.9.3-1
- Update to 2.9.3

* Thu Jun  8 2006 Matthias Clasen <mclasen@redhat.com> - 2.9.2-4
- Fix a crash in evolution

* Wed Jun  7 2006 Matthias Clasen <mclasen@redhat.com> - 2.9.2-3
- Fix the builtin icon cache

* Tue Jun  6 2006 Matthias Clasen <mclasen@redhat.com> - 2.9.2-2
- Add a BuildRequires for cups-devel
- configure with --disable-rebuilds

* Mon Jun  5 2006 Matthias Clasen <mclasen@redhat.com> - 2.9.2-1
- Update to 2.9.2

* Fri Jun  1 2006 Matthias Clasen <mclasen@redhat.com> - 2.9.1-2
- Rebuild

* Tue May 16 2006 Matthias Clasen <mclasen@redhat.com> - 2.9.1-1
- Update to 2.9.1

* Mon May  8 2006 Matthias Clasen <mclasen@redhat.com> - 2.9.0-4
- Bump required versions of GLib, Pango and cairo
- Add conflicts to force updating theme engine packages

* Fri May  5 2006 Matthias Clasen <mclasen@redhat.com> - 2.9.0-1
- Update to 2.9.0

* Fri Apr  7 2006 Matthias Clasen <mclasen@redhat.com> - 2.8.17-2
- Update to 2.8.17

* Thu Mar 30 2006 Matthias Clasen <mclasen@redhat.com> - 2.8.16-2
- Fix a multiscreen dnd crash

* Wed Mar 15 2006 Matthias Clasen <mclasen@redhat.com> - 2.8.16-1
- Update to 2.8.16

* Mon Mar 13 2006 Matthias Clasen <mclasen@redhat.com> - 2.8.15-1
- Update to 2.8.15
- Drop upstreamed patch

* Fri Mar 10 2006 Matthias Clasen <mclasen@redhat.com> - 2.8.14-2
- Fix a crash when using accessible treeviews

* Wed Mar  8 2006 Matthias Clasen <mclasen@redhat.com> - 2.8.14-1
- Update to 2.8.14 to fix a possible memory overrun
  in gtk_object_sink

* Sun Mar  5 2006 Matthias Clasen <mclasen@redhat.com> - 2.8.13-4
- Don't ship .la files for engines, either

* Wed Mar 01 2006 Karsten Hopp <karsten@redhat.de> 2.8.13-3
- Buildrequires: libXi-devel

* Mon Feb 27 2006 Ray Strode <rstrode@redhat.com> - 2.8.13-2
- s/Prereq/Requires/ for hicolor dep

* Sat Feb 25 2006 Matthias Clasen <mclasen@redhat.com> - 2.8.13-1
- Update to 2.8.13

* Fri Feb 24 2006 Ray Strode <rstrode@redhat.com> - 2.8.12-8
- add dependency on hicolor

* Sat Feb 11 2006 Matthias Clasen <mclasen@redhat.com> - 2.8.12-7.1
- Update to 2.8.12

* Fri Feb 10 2006 Jesse Keating <jkeating@redhat.com> - 2.8.11-7.1
- bump again for double-long bug on ppc(64)

* Wed Feb  9 2006 Matthias Clasen <mclasen@redhat.com> 2.8.11-7
- Fix a double free in the file chooser

* Tue Feb  7 2006 Christopher Aillon <caillon@redhat.com> 2.8.11-6
- Fix up jkeating's recent %%changelog entry to match this spec's style
- Make the devel package Require %%{version}-%%{release}

* Tue Feb  7 2006 Jesse Keating <jkeating@redhat.com> 2.8.11-5.1
- rebuilt for new gcc4.1 snapshot and glibc changes

* Mon Feb  6 2006 Matthias Clasen <mclasen@redhat.com> 2.8.11-5
- Sync render fix with upstream

* Fri Feb  3 2006 Matthias Clasen <mclasen@redhat.com> 2.8.11-3
- Avoid a slowpath in XRender

* Fri Jan 27 2006 Matthias Clasen <mclasen@redhat.com> 2.8.11-1
- Update to 2.8.11

* Thu Jan 19 2006 Christopher Aillon <caillon@redhat.com> 2.8.10-4
- Use Unicode character 2022 for the default invisible character

* Wed Jan 18 2006 Matthias Clasen <mclasen@redhat.com> 2.8.10-3
- Rebuild against GLib 2.9.4

* Fri Jan 13 2006 Matthias Clasen <mclasen@redhat.com> 2.8.10-2
- Run make check

* Thu Jan 12 2006 Matthias Clasen <mclasen@redhat.com> 2.8.10-1
- Update to 2.8.10

* Sat Dec 10 2005 Matthias Clasen <mclasen@redhat.com> 2.8.9-1
- Update to 2.8.9

* Fri Dec 09 2005 Jesse Keating <jkeating@redhat.com>
- rebuilt

* Tue Nov 28 2005 Matthias Clasen <mclasen@redhat.com> 2.8.8-1
- Update to 2.8.8

* Tue Nov 15 2005 Matthias Clasen <mclasen@redhat.com> 2.8.7-1
- Update to 2.8.7

* Tue Nov  8 2005 Matthias Clasen <mclasen@redhat.com> 2.8.6-6
- Clean up spec file a bit

* Mon Oct 31 2005 Matthias Clasen <mclasen@redhat.com> 2.8.6-5
- Switch requires to modular X

* Mon Oct 24 2005 Matthias Clasen <mclasen@redhat.com> 2.8.6-3
- Add a setting to hide the input method menu

* Wed Oct 19 2005 Matthias Clasen <mclasen@redhat.com> 2.8.6-2
- Sync to upstream xdgmime

* Wed Oct  5 2005 Matthias Clasen <mclasen@redhat.com> 2.8.6-1
- New upstream version

* Mon Oct  3 2005 Matthias Clasen <mclasen@redhat.com> 2.8.5-1
- New upstream version

* Fri Sep 30 2005 Matthias Clasen <mclasen@redhat.com> 2.8.4-2
- Prevent an overflow in size hints handling

* Tue Sep 27 2005 Matthias Clasen <mclasen@redhat.com> 2.8.4-1
- New upstream version

* Mon Aug 29 2005 Matthias Clasen <mclasen@redhat.com> 2.8.3-1
- Newer upstream version

* Mon Aug 15 2005 Matthias Clasen <mclasen@redhat.com> 2.8.0-1
- Newer upstream version

* Thu Aug  4 2005 Matthias Clasen <mclasen@redhat.com>
- Newer upstream version

* Thu Jul 28 2005 Owen Taylor <otaylor@redhat.com> 2.7.4-1
- Update to 2.7.4

* Fri Jul 15 2005 Matthias Clasen <mclasen@redhat.com>
- Update to 2.7.3

* Fri Jul  8 2005 Matthias Clasen <mclasen@redhat.com>
- Update to 2.7.2

* Fri Jul  1 2005 Matthias Clasen <mclasen@redhat.com>
- Update to 2.7.1

* Tue Jun 21 2005 Matthias Clasen <mclasen@redhat.com>
- update to 2.7.0
- bump requirements

* Tue May 10 2005 Matthias Clasen <mclasen@redhat.com>
- remove the openssl prereq again, as it did not fix
  Florians problem.

* Sun May  8 2005 Matthias Clasen <mclasen@redhat.com>
- remove debug spew

* Fri Apr 22 2005 Florian La Roche <laroche@redhat.com>
- add a Prereq: for the new openssl version to be installed first

* Wed Apr 13 2005 Matthias Clasen <mclasen@redhat.com> - 2.6.7-1
- Update to 2.6.7

* Mon Apr 11 2005 Matthias Clasen <mclasen@redhat.com> - 2.6.6-1
- Update to 2.6.6
- Drop upstreamed patches

* Sun Apr 10 2005 Jeremy Katz <katzj@redhat.com> - 2.6.5-2
- add patch from upstream CVS for broken icons (#154340, bgo#169870)

* Sat Apr  9 2005 Matthias Clasen <mclasen@redhat.com> - 2.6.5-1
- Update to 2.6.5
- Drop upstreamed patches

* Mon Mar 28 2005 Matthias Clasen <mclasen@redhat.com> - 2.6.4-3
- Fix a double free in the bmp loader

* Wed Mar  1 2005 Matthias Clasen <mclasen@redhat.com> - 2.6.4-2
- Rebuild with gcc4

* Tue Mar  1 2005 Matthias Clasen <mclasen@redhat.com> - 2.6.4-1
- Upgrade to 2.6.4
- Remove upstreamed patch

* Mon Feb 28 2005 Matthias Clasen <mclasen@redhat.com> - 2.6.3-1
- Upgrade to 2.6.3

* Fri Feb  4 2005 Matthias Clasen <mclasen@redhat.com> - 2.6.2-1
- Upgrade to 2.6.2

* Mon Jan 10 2005 Matthias Clasen <mclasen@redhat.com> - 2.6.1-1
- Upgrade to 2.6.1
- Drop no longer needed fixes

* Mon Dec 06 2004 Matthias Clasen <mclasen@redhat.com> - 2.4.14-1
- Upgrade to 2.4.14
- Remove the no longer needed pa.po patch
- Adjust gtk+-2.4.7-update-counter.patch

* Wed Dec 01 2004 Matthias Clasen <mclasen@redhat.com> - 2.4.13-13
- Revert an accidental ABI change.  (#151450)

* Wed Nov 03 2004 Matthias Clasen <mclasen@redhat.com> - 2.4.13-11
- Fix an oversight in the previous fix.

* Wed Nov 03 2004 Matthias Clasen <mclasen@redhat.com> - 2.4.13-8
- Fix an oversight in the previous fix, really
  fix the crash.  (#137922)

* Thu Oct 28 2004 Matthias Clasen <mclasen@redhat.com> - 2.4.13-5
- Include an upstream bugfix in the 
  gtk+-2.4.9-treeview-activate.patch. This fixes
  a crasher bug (#137461)

* Fri Oct 22 2004 Owen Taylor <otaylor@redhat.com> - 2.4.13-3
- Fix crash with backspace at end of buffer (#136840)

* Wed Oct 20 2004 Owen Taylor <otaylor@redhat.com> - 2.4.13-2
- Fix up backspace-deletes-character patches to actually work
  (#135656.)

* Wed Oct 20 2004 Matthias Clasen <mclasen@redhat.com> 
- Fix the translation of default:LTR in pa.po  (#136431)

* Tue Oct 12 2004 Matthias Clasen <mclasen@redhat.com> - 2.4.13-1
- Upgrade to 2.4.13

* Mon Oct 04 2004 Matthias Clasen <mclasen@redhat.com> - 2.4.10-7
- Don't move binaries to -32/-64 needlessly.

* Fri Oct 01 2004 Matthias Clasen <mclasen@redhat.com> - 2.4.10-6
- Fix a problem in the last patch.

* Tue Sep 28 2004 Matthias Clasen <mclasen@redhat.com> - 2.4.10-5
- Improve completion popup speed for large directories (#133313)

* Thu Sep 23 2004 Matthias Clasen <mclasen@redhat.com> - 2.4.10-4
- Make arrows in path bar larger.

* Wed Sep 22 2004 Matthias Clasen <mclasen@redhat.com> - 2.4.10-3
- Make SELECT_FOLDER work better in the file chooser.

* Wed Sep 15 2004 Matthias Clasen <mclasen@redhat.com> - 2.4.10-2
- don't install .la files.  (#132792)

* Wed Sep 15 2004 Matthias Clasen <mclasen@redhat.com> - 2.4.10-1
- update to latest upstream version, drop some patches

* Wed Sep 15 2004 Matthias Clasen <mclasen@redhat.com> - 2.4.9-9
- Fix issues in the xpm and ico loaders
  found by Chris Evans (#130711)

* Mon Sep 13 2004 Matthias Clasen <mclasen@redhat.com> - 2.4.9-8
- bring expanders back to their old size

* Fri Sep 10 2004 Matthias Clasen <mclasen@redhat.com> - 2.4.9-7
- backport support for PangoLogAttr.backspace_deletes_character

* Tue Sep  7 2004 Matthias Clasen <mclasen@redhat.com> - 2.4.9-6
- fix expander drawing (#131676)

* Thu Aug 26 2004 Matthias Clasen <mclasen@redhat.com> - 2.4.9-5
- prereq a new enough libtiff (#130678)

* Wed Aug 25 2004 Jonathan Blandford <jrb@redhat.com> 2.4.9-4
- backport patch to make typeahead activate the row

* Wed Aug 25 2004 Matthias Clasen <mclasen@redhat.com> - 2.4.9-3
- adjust patches

* Wed Aug 25 2004 Matthias Clasen <mclasen@redhat.com> - 2.4.9-1
- update to 2.4.9

* Tue Aug 24 2004 Soren Sandmann <sandmann@redhat.com> 2.4.7-4
- Backport update counter

* Tue Aug 24 2004 Jonathan Blandford <jrb@redhat.com> 2.4.7-2.3
- patch to make '/' do the search popup

* Fri Aug 20 2004 Owen Taylor <otaylor@redhat.com> - 2.4.7-2.2
- Fix problem with infinite loop on bad BMP data (#130450, 
  test BMP from Chris Evans, fix from Manish Singh)

* Sat Aug 14 2004 Matthias Clasen <mclasen@redhat.com> 2.4.7-1
- update to 2.4.7

* Fri Aug 13 2004 Matthias Clasen <mclasen@redhat.com> 2.4.6-1
- update to 2.4.6
- call libtoolize --force to win .so's back...

* Fri Jul 30 2004 Jonathan Blandford <jrb@redhat.com> 2.4.4-4
- add typeahead patch to GtkTreeView
- automake-1.9

* Tue Jul 27 2004 Matthias Clasen <mclasen@redhat.com> - 2.4.4-3
- Use -64 suffix on powerpc64.  (#128605)

* Fri Jul 16 2004 Matthias Clasen <mclasen@redhat.com> - 2.4.4-2
- Fix permissions of gdk-pixbuf-csource script. 
- Escape macros in %%changelog

* Fri Jul  9 2004 Matthias Clasen <mclasen@redhat.com> - 2.4.4-1
- Update to 2.4.4

* Thu Jul  8 2004 Matthias Clasen <mclasen@redhat.com> - 2.4.1-5
- Look for the gtk.immodules file in the right location.  (#127073)

* Thu Jul  8 2004 Matthias Clasen <mclasen@redhat.com> - 2.4.1-4
- Add a wrapper for gdk-pixbuf-csource.

* Wed Jun 23 2004 Matthias Clasen <mclasen@redhat.com> - 2.4.1-3
- Don't install testgtk and testtext
- Rename binaries to -32/-64 (#124478)
- Move arch-dependent config files to /etc/gtk-2.0/$host (#124482)
- Add wrappers for updating the arch-dependent config files

* Tue Jun 15 2004 Elliot Lee <sopwith@redhat.com>
- rebuilt

* Thu May 20 2004 Matthias Clasen <mclasen@redhat.com> - 2.4.1-1
- Upgrade to 2.4.1

* Wed Mar 17 2004 Alex Larsson <alexl@redhat.com> 2.4.0-1
- update to 2.4.0
- update bin_version to 2.4.0

* Wed Mar 10 2004 Mark McLoughlin <markmc@redhat.com> 2.3.6-1
- Update to 2.3.6
- Remove 2.3.5 buildfix patch
- Remove gdk-pixbuf-xlib dependancy fix

* Wed Mar 03 2004 Mark McLoughlin <markmc@redhat.com> 2.3.5-1
- Update to 2.3.5
- Bump the required glib and pango versions
- Make it build on x86_64

* Tue Mar 02 2004 Elliot Lee <sopwith@redhat.com>
- rebuilt

* Wed Feb 25 2004 Mark McLoughlin <markmc@redhat.com> 2.3.4-1
- Update to 2.3.4
- Remove the xft-prefs patch, its upstream now
- Don't kill libtool's hardcode_libdir_flag_spec anymore

* Fri Feb 13 2004 Elliot Lee <sopwith@redhat.com>
- rebuilt

* Fri Jan 23 2004 Alexander Larsson <alexl@redhat.com> 2.3.2-2
- Remove old HAVE_XFT2 check
- find_lang gtk20-properties too

* Fri Jan 23 2004 Jonathan Blandford <jrb@redhat.com> 2.3.2-1
- new version
- removed patches that have been applied to 2.3.x branch

* Mon Dec  1 2003 Thomas Woerner <twoerner@redhat.com> 2.2.4-5.2
- removed rpath

* Wed Oct 15 2003 Owen Taylor <otaylor@redhat.com> 2.2.4-5.1
- Link gdk-pixbuf-xlib against gdk-pixbuf (#106678)

* Fri Oct  3 2003 Owen Taylor <otaylor@redhat.com> 2.2.4-4.0
- Fix 64-bit problem in gtkimcontextxim.c (#106124)

* Tue Sep 16 2003 Owen Taylor <otaylor@redhat.com> 2.2.4-3.0
- Fix an infinite loop that can occur in the panel (#104524)

* Fri Sep  5 2003 Owen Taylor <otaylor@redhat.com> 2.2.4-2.1
- Fix up tutorial in packaging (#90197), add FAQ 
- Back out change to make KP_Decimal interpretation dependent on locale
  (#101046)

* Thu Sep  4 2003 Owen Taylor <otaylor@redhat.com> 2.2.4-1.1
- Version 2.2.4 - fixes a few small problems in 2.2.3

* Tue Aug 26 2003 Owen Taylor <otaylor@redhat.com> 2.2.3-1.1
- Version 2.2.3

* Thu Jul 10 2003 Owen Taylor <otaylor@redhat.com> 2.2.2-2.0
- Change release number for rebuild

* Wed Jul  9 2003 Owen Taylor <otaylor@redhat.com> 2.2.2-2.1
- XFlush() rather than XSync() at the end of process_all_updates()
  (big remote X anaconda speedup)
- Add patch to fix frequent Red Hat 9 crash 
  http://bugzilla.gnome.org/show_bug.cgi?id=105745

* Mon Jun  9 2003 Owen Taylor <otaylor@redhat.com>
- Version 2.2.2
- Mark assembly files as noexec-stack

* Wed Jun 04 2003 Elliot Lee <sopwith@redhat.com>
- rebuilt

* Mon Feb 24 2003 Jonathan Blandford <jrb@redhat.com> 2.2.1-2
- add a libpng dependency to pull in the rebuilt version.

* Fri Feb 21 2003 Jonathan Blandford <jrb@redhat.com> 2.2.1-2
- add a patch to fix broken scrolling in a lot of applications.

* Sun Feb  2 2003 Owen Taylor <otaylor@redhat.com>
- Version 2.2.1
- Update xftprefs for gtk+-2.2.1

* Wed Jan 22 2003 Tim Powers <timp@redhat.com>
- rebuilt

* Tue Jan 14 2003 Jonathan Blandford <jrb@redhat.com>
- patch to fix TreeView misdrawing.  Remove when 2.2.1 comes out

* Fri Dec 20 2002 Owen Taylor <otaylor@redhat.com>
- Version 2.2.0

* Fri Dec 20 2002 Nalin Dahyabhai <nalin@redhat.com>
- Fix postun to not try to run a script through ldconfig
- Only remove the gtk.immodules and gdk-pixbuf.loaders files if uninstalling
  while not upgrading

* Wed Dec 11 2002 Owen Taylor <otaylor@redhat.com>
- Version 2.1.5

* Wed Dec 11 2002 Owen Taylor <otaylor@redhat.com>
- Version 2.1.4

* Wed Dec  4 2002 Owen Taylor <otaylor@redhat.com>
- Fix problem with GtkCombo not setting text to first item

* Tue Dec  3 2002 Owen Taylor <otaylor@redhat.com>
- Version 2.1.3, re-add xftprefs patch

* Fri Nov 22 2002 Havoc Pennington <hp@redhat.com>
- rebuild with xft support

* Wed Nov 20 2002 Havoc Pennington <hp@redhat.com>
- rebuild to hack around xft.pc being in the wrong place
- buildreq the pango with pangoxft

* Thu Nov  7 2002 Havoc Pennington <hp@redhat.com>
- 2.1.3
- remove TODO from doc, no longer exists
- remove 64bit patch, now upstream
- comment out scroll_to patch, jrb has to deal with this
- remove keycode patch now upstream
- remove usintl patch now upstream
- remove imenvar patch, now upstream
- remove xftprefs patch now upstream
- remove xftdraw patch now upstream
- remove installdir patch (no longer applies) and do "makeinstall RUN_QUERY_IMMODULES_TEST=false"
- remove extranotify patch, now upstream
- add gdk-pixbuf-query-loaders to file list
- remove gdk-pixbuf.loaders and gtk.immodules in postun as they are
  not owned by the package (these should probably live in /var since they 
  aren't config files and we overwrite them all the time)

* Thu Oct  3 2002 Owen Taylor <otaylor@redhat.com>
- Add a fix for a 64bit problem in gtktypeutils.h
- Bump in rebuild for RPM configuration problem

* Sun Aug 25 2002 Jonathan Blandford <jrb@redhat.com>
- fix gtk_tree_view_scroll_to_cell

* Fri Aug 23 2002 Owen Taylor <otaylor@redhat.com>
- Fixed Raleigh theme missing from package list

* Mon Aug 19 2002 Owen Taylor <otaylor@redhat.com>
- Fix a memory leak in xftprefs.patch
- Fix extra settings notifies on startup that were causing significant
  performance problems as fonts were reloaded.

* Tue Aug 13 2002 Owen Taylor <otaylor@redhat.com>
- Fixes to GtkIMContextSimple compose table for us-intl keyboards
  (#70995, Alexandre Oliva)
- Fix problem with keycodes passed to GtkIMContextXIM
	
* Thu Aug  8 2002 Owen Taylor <otaylor@redhat.com>
- Remove fixed-ltmain.sh, no longer needed
- Fix bug with GTK_IM_MODULE environment variable
- Remove profile.d entries setting GDK_USE_XFT, since we now default to it on
- Backport patch from CVS HEAD to get Xft to work on non-RENDER XServers
- Version 2.0.6

* Tue Jul 16 2002 Owen Taylor <otaylor@redhat.com>
- Fix cut and paste error in xftprefs patch pointed out by Anders Carlsson

* Mon Jul  8 2002 Owen Taylor <otaylor@redhat.com>
- Add patch to hook Xft up to XSETTINGS

* Tue Jul  2 2002 Jonathan Blandford <jrb@redhat.com>
- tree-view fixes for anaconda.  Already in CVS.

* Fri Jun 21 2002 Owen Taylor <otaylor@redhat.com>
- Default GDK_USE_XFT to on, not off

* Sun Jun 16 2002 Havoc Pennington <hp@redhat.com>
- 2.0.5
- remove xft configure.in patch

* Fri Jun 07 2002 Havoc Pennington <hp@redhat.com>
- rebuild in different environment

* Fri Jun  7 2002 Havoc Pennington <hp@redhat.com>
- rebuild

* Thu Jun  6 2002 Owen Taylor <otaylor@redhat.com>
- Add patch so that configuration works with pango-1.1/fontconfig

* Tue Jun  4 2002 Havoc Pennington <hp@redhat.com>
- 2.0.3

* Mon Jun 03 2002 Havoc Pennington <hp@redhat.com>
- rebuild in different environment

* Mon Jun  3 2002 Havoc Pennington <hp@redhat.com>
- drop /etc/gtk-2.0/gtkrc from the file list, will now be provided by redhat-artwork

* Wed May 29 2002 Havoc Pennington <hp@redhat.com>
- rebuild in different environment

* Wed May 29 2002 Havoc Pennington <hp@redhat.com>
- add profile.d entries to set GDK_USE_XFT

* Thu May 23 2002 Tim Powers <timp@redhat.com>
- automated rebuild

* Thu Apr 25 2002 Havoc Pennington <hp@redhat.com>
- rebuild in different environment
- hardcode automake 1.4 req

* Fri Apr 19 2002 Havoc Pennington <hp@redhat.com>
- do the prefix/lib -> libdir thing
- include key themes in the package

* Mon Apr 15 2002 root <otaylor@redhat.com>
- Fix missing .po files (#63336)

* Thu Apr 11 2002 Owen Taylor <otaylor@redhat.com>
- Add reference docs to -devel package (#61184)
- Use GTK2_RC_FILES, not GTK_RC_FILES, since KDE points GTK_RC_FILES 
  to gtk-1.2 ~/.gtkrc

* Wed Apr  3 2002 Alex Larsson <alexl@redhat.com>
- Change dependency for glib2 since gtk and glib versions mismatch

* Wed Apr  3 2002 Alex Larsson <alexl@redhat.com>
- Update to version 2.0.2

* Fri Mar  8 2002 Owen Taylor <otaylor@redhat.com>
- Version 2.0.0

* Mon Feb 25 2002 Alex Larsson <alexl@redhat.com>
- Update to 1.3.15

* Thu Feb 21 2002 Alex Larsson <alexl@redhat.com>
- Bump for rebuild

* Mon Feb 18 2002 Alex Larsson <alexl@redhat.com>
- Update to 1.3.14

* Fri Feb 15 2002 Havoc Pennington <hp@redhat.com>
- add horrible buildrequires hack

* Thu Feb 14 2002 Havoc Pennington <hp@redhat.com>
- 1.3.13.91 snapshot

* Mon Feb 11 2002 Matt Wilson <msw@redhat.com>
- build from CVS snapshot
- use setup -q

* Wed Jan 30 2002 Owen Taylor <otaylor@redhat.com>
- Version 1.3.13

* Tue Jan 22 2002 Havoc Pennington <hp@redhat.com>
- automake14

* Wed Jan  2 2002 Havoc Pennington <hp@redhat.com>
- 1.3.12.90 snapshot

* Sun Nov 25 2001 Havoc Pennington <hp@redhat.com>
- Version 1.3.11
- check atk/pango versions explicitly prior to build,
  so that --nodeps tricks don't result in bad binary RPMs

* Fri Oct  5 2001 Havoc Pennington <hp@redhat.com>
- pixbuf loaders were missing from file list
- conflict with gdk-pixbuf-devel <= 0.11

* Thu Oct  4 2001 Havoc Pennington <hp@redhat.com>
- cvs snap

* Thu Sep 27 2001 Havoc Pennington <hp@redhat.com>
- sync with Owen's version

* Thu Sep 20 2001 Havoc Pennington <hp@redhat.com>
- smp_mflags
- langify

* Wed Sep 19 2001 Havoc Pennington <hp@redhat.com>
- 1.3.8
- add automake hackarounds

* Thu Sep 13 2001 Havoc Pennington <hp@redhat.com>
- conflict with old GTK with headers not moved
- prereq new version of pango

* Mon Sep 10 2001 Havoc Pennington <hp@redhat.com>
- update to CVS snapshot

* Wed Sep  5 2001 Havoc Pennington <hp@redhat.com>
- build require specific versions of dependencies

* Tue Sep  4 2001 Owen Taylor <otaylor@redhat.com>
- Version 1.3.7

* Thu Jul 26 2001 Havoc Pennington <hp@redhat.com>
- Obsolete Inti and Inti-devel, #49967

* Sat Jul 21 2001 Owen Taylor <otaylor@redhat.com>
- PreReq specific pango and atk versions (#49434)
- Don't package gtk.immodules (#49584)
- Added BuildPrereq for libtiff-devel, libjpeg-devel, libpng-devel (#49495)
- Configure with --disable-gtk-doc (#48987)
- Package libgdk_pixbuf_xlib (#47753)

* Sat Jul  7 2001 Tim Powers <timp@redhat.com>
- languify to satisfy rpmlint

* Thu Jun 21 2001 Florian La Roche <Florian.LaRoche@redhat.de>
- use something better than libtool

* Wed Jun 13 2001 Havoc Pennington <hp@redhat.com>
- 1.3.6
- libtool hackery
- obsolete gtk+-gtkbeta-devel

* Fri May  4 2001 Owen Taylor <otaylor@redhat.com>
- Version 1.3.5
- Rename to gtk2

* Fri Nov 17 2000 Owen Taylor <otaylor@redhat.com>
- Final 1.3.2

* Tue Nov 14 2000 Owen Taylor <otaylor@redhat.com>
- New snapshot

* Mon Nov 13 2000 Owen Taylor <otaylor@redhat.com>
- 1.3.2pre1 snapshot version

* Sun Aug 13 2000 Owen Taylor <otaylor@redhat.com>
- Rename to 1.3.1b to avoid version increment difficulties

* Thu Aug 10 2000 Havoc Pennington <hp@redhat.com>
- Fix .pc files to not contain -I%%{_includedir}

* Thu Aug 10 2000 Havoc Pennington <hp@redhat.com>
- Update to a CVS snapshot

* Fri Jul 14 2000 Owen Taylor <otaylor@redhat.com>
- Removed stray b from %%postun
- Real 1.3.1 tarball fixing stupid omission in gtk-config

* Fri Jul 07 2000 Owen Taylor <otaylor@redhat.com>
- Version 1.3.1
- move back to /usr
- Remove gtk-config.1 manpage from build since
  it conflicts with gtk+-devel. When we go to 
  gtk+ gtk+1.2 setup, we should add it back

* Fri Jun 30 2000 Owen Taylor <otaylor@redhat.com>
- Rename gtkrc-default source so that it GTK+ package can't remove it

* Thu Jun 8 2000  Owen Taylor <otaylor@redhat.com>
- Rebuild with a prefix of /opt/gtk-beta

* Wed May 31 2000 Owen Taylor <otaylor@redhat.com>
- New version

* Tue Apr 25 2000 Owen Taylor <otaylor@redhat.com>
- Snapshot version to install in /opt/pango

* Mon Feb 21 2000 Owen Taylor <otaylor@redhat.com>
- Fix weird excess  problem that somehow turned up in %%{_sysconfdir}/gtkrc.LANG

* Mon Feb 14 2000 Owen Taylor <otaylor@redhat.com>
- More patches from 1.2.7

* Fri Feb 04 2000 Owen Taylor <otaylor@redhat.com>
- Set the charset explicitely for the default font to avoid
  problems with XFree86-4.0 where the default charset is
  iso10646-1, not iso8859-1.
- Fix problems with size requisitions for scrolled windows
  that was causing looping. (RH bug #7997)

* Thu Feb 03 2000 Owen Taylor <otaylor@redhat.com>
- Explicitely set the foreground of the tooltips to black
  to avoid bad interactions with themes that set a
  light foreground color.

* Thu Feb 03 2000 Owen Taylor <otaylor@redhat.com>
- Added large patch of bugfixes in stable branch of CVS

* Tue Oct 12 1999 Owen Taylor <otaylor@redhat.com>
- Added Akira Higuti's patch for line-wrapping in GTK+

* Thu Oct 7  1999 Owen Taylor <otaylor@redhat.com>
- version 1.2.6

* Thu Sep 23 1999 Owen Taylor <otaylor@redhat.com>
- version 1.2.5
- install tutorial GIFs

* Wed Sep 22  1999 Owen Taylor <otaylor@redhat.com>
- Upgrade to real 1.2.5pre2
- Changed name so upgrade to 1.2.5 will work :-(
- Add extra gtkrc files
- Add examples and English language tutorial to -devel package

* Fri Sep 17 1999 Owen Taylor <otaylor@redhat.com>
- Upgraded to 1.2.5pre2. (Actually, pre-pre-2)

* Tue Aug 17 1999 Michael Fulbright <drmike@redhat.com>
- added threaded patch

* Mon Jun 7 1999 Owen Taylor <otaylor@redhat.com>
- Update for GTK+-1.2.3
- Patches that will be in GTK+-1.2.4
- Patch to keep GTK+ from coredumping on X IO errors
- Patch to improve compatilibity with GTK-1.2.1 (allow
  event mask to be set on realized widgets)

* Mon Apr 19 1999 Michael Fulbright <drmike@redhat.com>
- fixes memory leak

* Mon Apr 12 1999 Owen Taylor <otaylor@redhat.com>
- The important bug fixes that will be in GTK+-1.2.2

* Thu Apr 01 1999 Michael Fulbright <drmike@redhat.com>
- patches from owen to handle various gdk bugs

* Sun Mar 28 1999 Michael Fulbright <drmike@redhat.com>
- added XFree86-devel requirement for gtk+-devel

* Thu Mar 25 1999 Michael Fulbright <drmike@redhat.com>
- version 1.2.1

* Wed Mar 17 1999 Michael Fulbright <drmike@redhat.com>
- removed /usr/info/dir.gz file from package

* Fri Feb 26 1999 Michael Fulbright <drmike@redhat.com>
- Version 1.2.0

* Thu Feb 25 1999 Michael Fulbright <drmike@redhat.com>
- version 1.2.0pre2, patched to use --sysconfdir=%%{_sysconfdir}

* Mon Feb 15 1999 Michael Fulbright <drmike@redhat.com>
- patched in Owen's patch to fix Metal theme

* Fri Feb 05 1999 Michael Fulbright <drmike@redhat.com>
- bumped up to 1.1.15

* Wed Feb 03 1999 Michael Fulbright <drmike@redhat.com>
- bumped up to 1.1.14

* Mon Jan 18 1999 Michael Fulbright <drmike@redhat.com>
- bumped up to 1.1.13

* Wed Jan 06 1999 Michael Fulbright <drmike@redhat.com>
- bumped up to 1.1.12

* Wed Dec 16 1998 Michael Fulbright <drmike@redhat.com>
- added Theme directory to file list
- up to 1.1.7 for GNOME freeze

* Sun Oct 25 1998 Shawn T. Amundson <amundson@gtk.org>
- Fixed Source: to point to v1.1 

* Tue Aug 04 1998 Michael Fulbright <msf@redhat.com>
- change %%postun to %%preun

* Mon Jun 27 1998 Shawn T. Amundson
- Changed version to 1.1.0

* Thu Jun 11 1998 Dick Porter <dick@cymru.net>
- Removed glib, since it is its own module now

* Mon Apr 13 1998 Marc Ewing <marc@redhat.com>
- Split out glib package

* Tue Apr  8 1998 Shawn T. Amundson <amundson@gtk.org>
- Changed version to 1.0.0

* Tue Apr  7 1998 Owen Taylor <otaylor@gtk.org>
- Changed version to 0.99.10

* Thu Mar 19 1998 Shawn T. Amundson <amundson@gimp.org>
- Changed version to 0.99.9
- Changed gtk home page to www.gtk.org

* Thu Mar 19 1998 Shawn T. Amundson <amundson@gimp.org>
- Changed version to 0.99.8

* Sun Mar 15 1998 Marc Ewing <marc@redhat.com>
- Added aclocal and bin stuff to file list.
- Added -k to the SMP make line.
- Added lib/glib to file list.

* Fri Mar 14 1998 Shawn T. Amundson <amundson@gimp.org>
- Changed version to 0.99.7

* Fri Mar 14 1998 Shawn T. Amundson <amundson@gimp.org>
- Updated ftp url and changed version to 0.99.6

* Thu Mar 12 1998 Marc Ewing <marc@redhat.com>
- Reworked to integrate into gtk+ source tree
- Truncated ChangeLog.  Previous Authors:
  Trond Eivind Glomsrod <teg@pvv.ntnu.no>
  Michael K. Johnson <johnsonm@redhat.com>
  Otto Hammersmith <otto@redhat.com>
  
