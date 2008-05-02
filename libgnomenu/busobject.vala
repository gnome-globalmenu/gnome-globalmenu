
using GLib;

namespace Gnomenu {
public class BusObject:Object {
	private weak BusObject _parent;
	private string _title;
	protected string _path;
	private bool _exposed;
	private bool _visible;
	public weak BusObject parent {
		get {
			return _parent;
		}
		set {
			message("set parent");
			_parent = value;
			reset_path();
			if(_parent is BusObject)
				if(_parent._exposed) this.expose();
			notify("parent");
		}
	}
	public string title {get{return _title;} set{_title=value;notify("title");}}
	public string path {get{return _path;}}
	public string name {get; construct;} /*read-only, unique*/
	public bool visible {get{return _visible;} set{_visible=value;notify("visible");}}
	public signal void prop_changed(string prop);
	construct {
		_title = name;
		_parent = null;
		_path = name;
		_exposed = false;
		_visible = false;
	}
	public void notify(string prop){
			prop_changed(prop);
	}
	public virtual void expose() {
		message("path = %s", path);
		if(conn == null) {
			message("connection fails, do not expose");
			return;
		}
		Object o = conn.lookup_object(path);
		if( o is Object) {
			if( o == this ) {
				message("%s is already exposed at: %s", name, path);
				return;
			} else {
				message("remove the old object at:%s", path);
				o.unref();
			}
		}
		conn.register_object(path, this);
		_exposed = true;
	}
	public virtual void reset_path(){
		if(_parent is BusObject) {
			_path = _parent.path + "/" + name;
		} else {
			message("parent is not a BusObject??");
		}

	}
	public virtual bool getVisible(){
		return _visible;
	}
	public virtual string getTitle(){
		return _title;
	}
}
}
