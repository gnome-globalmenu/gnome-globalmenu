
using GLib;

namespace Gnomenu {
public class BusObject:Object {
	private weak BusObject _parent;
	private string _title;
	protected string _path;
	protected bool _exposed; /*application.vala*/
	private bool _visible;
	private bool _enabled;
	private DBus.Connection _conn;
	public DBus.Connection conn {
		set {
			DBus.Connection __conn = _conn;
			_conn = value;
			if(_exposed && __conn != _conn) {
				reexpose();
			}
		}
		get { return _conn; }
	}
	[Notify]
	public weak BusObject parent {
		get {
			return _parent;
		}
		set {
			message("set parent");
			_parent = value;
			reset_path();
			if(_parent is BusObject && _parent._exposed) this.expose();
		}
	}
	public string path {get{return _path;}} /*read-only unique*/
	public string name {get; construct;} /*read-only, not unique*/

	[Notify]
	public string title {get{return _title;} set{_title=value;/*notify("title");*/}}
	[Notify]
	public bool visible {get{return _visible;} set{_visible=value;/*notify("visible");*/}}
	[Notify]
	public bool enabled {get{return _enabled;} set{_enabled=value;}}

	public signal void prop_changed(string prop);
	construct {
		_title = name;
		_parent = null;
		_path = encode_name(name);
		_exposed = false;
		_visible = false;
		_enabled = true;
		base.notify += (sender, ps) => {
			prop_changed(ps.name);
		};
	}

	public void expose() {
		if(_parent != null)
			this.conn = _parent.conn;
		if(_exposed){
			message("already exposed");
			return;
		}
		_exposed = true;
		reexpose();
		@foreach(expose);
	}
	public static delegate void Func(BusObject obj);
	protected virtual void @foreach(BusObject.Func cb) {
	}
	protected virtual void reexpose() {
		if(!_exposed) return;
		message("reexpose");
		message("path = %s", path);
		bool ok = false;
		var test_path = path;
		int id = 0;
		if(conn == null) {
			message("connection fails, do not expose");
			return;
		}
		while (!ok){
			Object o = conn.lookup_object(test_path);
			if( o is Object) {
				if( o == this ) {
					message("%s is already exposed at: %s", name, test_path);
					return;
				}
			}  
			if( o  == null){
				ok = true;
				break;
			}
			test_path = "%s%d".printf(path, id++);
			message("test path = %s", test_path);
		}
		conn.register_object(test_path, this);
	}
	private virtual void reset_path(){
		if(_parent is BusObject) {
			_path = _parent.path + "/" + encode_name(name);
		} else {
			message("parent is not a BusObject??");
			_path = encode_name(name);
		}
		@foreach(reset_path);
	}
	public virtual bool getVisible(){
		return _visible;
	}
	public virtual string getTitle(){
		return _title;
	}
	public virtual bool getEnabled(){
		return _enabled;
	}
}
}
