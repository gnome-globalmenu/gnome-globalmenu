#! /bin/sh
libexec_dir=$1
cat <<EOF
<oaf_info>
  <oaf_server iid="OAFIID:GNOME_GlobalMenuApplet2_Factory" type="exe"
    location="${libexec_dir}/gnome2-globalmenu-applet">
    <oaf_attribute name="repo_ids" type="stringv">
      <item value="IDL:Bonobo/GenericFactory:1.0"/>
      <item value="IDL:Bonobo/Unknown:1.0"/>
    </oaf_attribute>
    <oaf_attribute name="name" type="string" value="GlobalMenu applet Factory"/>
    <oaf_attribute name="description" type="string" value="Global Menu applet factory"/>
  </oaf_server>
  <oaf_server iid="OAFIID:GNOME_GlobalMenuApplet2" type="factory"
    location="OAFIID:GNOME_GlobalMenuApplet2_Factory">
    <oaf_attribute name="repo_ids" type="stringv">
      <item value="IDL:GNOME/Vertigo/PanelAppletShell:1.0"/>
      <item value="IDL:Bonobo/Control:1.0"/>
      <item value="IDL:Bonobo/Unknown:1.0"/>
    </oaf_attribute>
    <oaf_attribute name="name" type="string" value="Global Menubar"/>
    <oaf_attribute name="description" type="string" value="Global Menubar applet for gtk2-aqd"/>
    <oaf_attribute name="panel:icon" type="string" value="gnome-fs-home"/>
  </oaf_server>
</oaf_info>
EOF
