using GLib;
using Gtk;
using GMarkupDoc;
using Gnomenu;
using DBus;
namespace POSIX {
const int SIGQUIT = 3;
[CCode (cname = "kill")]
public extern int kill(uint pid, int sig);
}
public class Application {
	private static bool viewer = false;
	private static bool kill = false;
	const OptionEntry[] options = {
		{"viewer", 'v',	0, OptionArg.NONE, ref viewer, "display a viewer", null},
		{"kill", 'k', 0, OptionArg.NONE, ref kill, "kill the server", null},
		{null}
	};
	public static int main(string[] args) {
		OptionContext opt_context = new OptionContext ("- org.gnome.GlobalMenu.Server");
		opt_context.set_help_enabled (true);
		opt_context.add_main_entries (options, null);
		opt_context.add_group(Gtk.get_option_group(false));
		opt_context.parse (ref args);
		Gtk.init(ref args);
		if(kill) {
			try {
				var conn = Bus.get(DBus.BusType.SESSION);
				dynamic DBus.Object dbus = conn.get_object("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus");
				string old_owner = dbus.GetNameOwner("org.gnome.GlobalMenu.Server");
				uint pid  = dbus.GetConnectionUnixProcessID(old_owner);
				message("killing old service at %s with pid = %u", old_owner, pid);
				return POSIX.kill(pid, POSIX.SIGQUIT);
			} catch (GLib.Error e){
				message("%s", e.message);
				return 255;
			}
		}
		MainLoop loop = new MainLoop(null, false);
		Document document = new Document();
		Server c = new Server(document);
		if(viewer) {
			Gtk.Window window = new Window(Gtk.WindowType.TOPLEVEL);
			window.add(new ListView(document));
			window.show_all();
		}
		loop.run();
		return 0;
	}

}

