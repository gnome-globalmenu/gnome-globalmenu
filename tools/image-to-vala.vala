using Gtk;

[CCode (array_length = false, array_null_terminated = true)]
public string [] filenames = null;
[CCode (array_length = false, array_null_terminated = true)]
public string [] names = null;
public string @namespace = null;
public string @class = null;
const OptionEntry[] options = {
	{"namespace", 'N',0, OptionArg.STRING, ref @namespace, "namespace name", null},
	{"class", 'C',0, OptionArg.STRING, ref @class, "class name", null},
	{"name", 'n', 0, OptionArg.STRING_ARRAY, ref names, "variable names", null},
	{"", 0, 0, OptionArg.FILENAME_ARRAY, ref filenames, "filenames", null},
	{null}
};

string pixbuf_encode_b64(Gdk.Pixbuf pixbuf) {
	Gdk.Pixdata pixdata = {0};
	pixdata.from_pixbuf(pixbuf, true);
	return Base64.encode(pixdata.serialize());
}

public int main(string[] args) {
	OptionContext context = new OptionContext(" - Convert image to gnomenu string");
	context.add_main_entries(options, null);
	context.parse(ref args);
	int i;
	if(@namespace != null) {
		stdout.printf("namespace %s {\n", @namespace);
	}
	if(@class != null) {
		stdout.printf("\tclass %s {\n", @class);	
	}
	for(i = 0; filenames!=null && filenames[i] != null; i++) {
		string filename = filenames[i];
		string name = names[i];
		try {
			Gdk.Pixbuf pixbuf = new Gdk.Pixbuf.from_file(filename);
			string str = pixbuf_encode_b64(pixbuf);
			if(@class != null) {
				stdout.printf("\t\tpublic static ");	
			} else {
				stdout.printf("\tpublic ");
			}
			stdout.printf("const string %s = \"\"\"pixbuf:%s\"\"\";\n",
					names[i], str);
		} catch(GLib.Error e) {
			warning("%s", e.message);
		}
	}
	if(@class != null) {
		stdout.printf("\t}\n");	
	}
	if(@namespace != null) {
		stdout.printf("}\n");
	}
	return 0;
}
