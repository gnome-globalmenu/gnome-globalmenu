/* -*- Mode: Vala; indent-tabs-mode: nil; tab-width: 2 -*- */

using GLib;

public class MainWindow : Gtk.Window
{
  construct {
    Gtk.VBox vb = new Gtk.VBox (false, 0);
    
    this.title = "Testing";
    this.resizable = false;
    
    var align = new Gtk.Alignment(0.5f, 0.5f, 0.0f, 0.0f);
    
    var button = new Gtk.Button();
    button.set("child", align);
    
    var icon = new Gtk.Image();
    try {
      var theme = Gtk.IconTheme.get_for_screen(get_screen());
      var pix = theme.load_icon("preferences-system", 128,
                                Gtk.IconLookupFlags.FORCE_SIZE);
      icon.set("pixbuf", pix);
    }
    catch (Error e) {
      warning("%s\n", e.message);
    }
    
    var hbox = new Gtk.HBox(true, 12);
    hbox.set("border-width", 12,
             "child", icon);
    
    vb.pack_start (setup_menu (), false, false, 0);
    vb.pack_start (hbox, true, true, 0);
    
    add (vb);
  }
  
  Gtk.Widget setup_menu ()
  {
    var action_group = new Gtk.ActionGroup ("actions");
    
    var action = new Gtk.Action ("FileMenuAction", "File", null, null);
    action_group.add_action (action);
    
    action = new Gtk.Action ("QuitAction", null, null, Gtk.STOCK_QUIT);
    action.activate.connect(Gtk.main_quit);
    action_group.add_action_with_accel (action, "<control>Q");
    
    var ui = """
<ui>
  <menubar>
    <menu name="FileMenu" action="FileMenuAction">
      <menuitem name="Quit" action="QuitAction" />
    </menu>
  </menubar>
</ui>""";

    var manager = new Gtk.UIManager ();
    try {
    manager.add_ui_from_string (ui, (ssize_t)ui.size ());
    } catch (Error e)  {
      error ("Internal error: bad ui string.\n");
    }
    manager.insert_action_group (action_group, 0);
    add_accel_group (manager.get_accel_group ());
    
    return manager.get_widget ("/ui/menubar");
  }

}
static int main(string[] args)
{
  Gtk.init(ref args);
  Idle.add(() => {
    warning("main_quit!");
    Gtk.main_quit();
    return false;
  });
  Gtk.main();

 var win = new MainWindow();

  win.show_all();
  Gtk.main();
  return 0;
}
