
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
	protected override void @foreach(BusObject.Func func){
		if(_menu != null) {
			func(_menu);
		}
	}
	public virtual string getMenu(){
		if(menu is Menu) return menu.path;
		return "";
	}
}
}
