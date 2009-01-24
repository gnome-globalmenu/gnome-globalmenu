const Gtk = imports.gi.Gtk;
const Gnomenu = imports.gi.Gnomenu;
const Wnck = imports.gi.Wnck;

Gtk.init(0, null);
let win = new Gtk.Window({type: Gtk.WindowType.TOPLEVEL});

let box = new Gtk.VBox({homogeneous: false, spacing: 0});
let screen = Wnck.Screen.get_default();

win.add(box);
let globalmenu = new Gnomenu.GlobalMenu();
log(box);
function closure (prev) {
	log("closure");
	let win = screen.get_active_window();
	if(win != null) 
		globalmenu.switch_to(win.get_xid());
	else
		log("win is null");
}
box.add(globalmenu);
screen.connect("active-window-changed", 
		closure
		);
win.show_all();
Gtk.main();

