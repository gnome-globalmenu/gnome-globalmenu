
using GLib;
namespace Gnomenu {
public class MenuOwner: BusObject {
	private Menu _menu;
	public Menu menu {
		get{
			return _menu;
		}
		set {
			_menu = value;
			message("%d", value);
			if(_menu is BusObject)
				_menu.parent = this;
			prop_changed("menu");
		}
	}
	public override void expose() {
		base.expose();
		if(menu is Menu) menu.expose();
	}
	public override void reset_path() {
		base.reset_path();
		if(menu is Menu) menu.reset_path();
	}
	public virtual string getMenu(){
		if(menu is Menu) return menu.path;
		return "";
	}
}
}
