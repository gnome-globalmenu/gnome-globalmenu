using Gtk;

namespace Gnomenu {

	public class MenuLabel: Gtk.Container {
		public MenuLabel() {
			_accel_widget = new Label("");
			_accel_widget.visible = false;
			_label_widget = new Label("");
			_label_widget.visible = false;
			_label_widget.use_underline = true;
			add(_label_widget);
			add(_accel_widget);
			child_set(_accel_widget, "alignment", Pango.Alignment.RIGHT, null);
			child_set(_accel_widget, "padding", 10, null);
		}
		private struct ChildPropBag {
			public Pango.Alignment alignment;
			public int padding;
		}
		static const int PROP_ALIGNMENT = 1234;
		static const int PROP_PADDING = 1235;
		static construct {
			install_child_property(PROP_ALIGNMENT,
					new ParamSpecEnum(
						"alignment",
						"Alignment",
						"the alignment of the child",
						typeof(Pango.Alignment),
						Pango.Alignment.CENTER,
						ParamFlags.READABLE |
						ParamFlags.WRITABLE
				));
			install_child_property(PROP_PADDING,
					new ParamSpecInt(
						"padding",
						"Padding",
						"the padding on left, both or right",
						0, 1000, 0,
						ParamFlags.READABLE |
						ParamFlags.WRITABLE
				));
		}
		public string accel {
			get {
				return _accel;
			}
			set {
				if(_accel == value) return;
				_accel = value;
				if(value ==  null) {
					_accel_widget.visible = false;
				} else {
					_accel_widget.label = value;
					_accel_widget.visible = true;
				}
				queue_resize();
			}
		}
		public bool use_underline {
			get {
				return _use_underline;
			}
			set {
				if(_use_underline == value) return;
				_use_underline = value;
				_label_widget.use_underline = _use_underline;
			}
		}
		public string label {
			get {
				return _label;
			}
			set {
				if(_label == value) return;
				_label = value;
				if(value ==  null) {
					_label_widget.visible = false;
				} else {
					_label_widget.label = value;
					_label_widget.visible = true;
				}
				queue_resize();
			}
		}

		public Gravity gravity {
			get {
				return _gravity;
			}
			set {
				if(_gravity == value) return;
				_gravity = value;
				foreach(Label child in children)
					update_label_gravity(child);
				queue_resize();
			}
		}
		public Gtk.Label? label_widget {
			get {
				return _label_widget;
			}
		}
		construct {
			props = new HashTable<weak Widget, ChildPropBag*>.full(direct_hash, direct_equal,
				null, free);
			set_flags(WidgetFlags.NO_WINDOW);
		}
		public override void add(Widget child) {
			if(!(child is Label)) {
				warning("only GtkLabel is accepted");
				return;
			}
			children.append(child as Label);
			child.set_parent(this);
			props.insert(child, (ChildPropBag*)malloc0(sizeof(ChildPropBag)));
			update_label_gravity(child as Label);
		}
		public override void remove(Widget child) {
			children.remove_all(child as Label);
			child.unparent();
			props.remove(child);
		}

		private Label _label_widget;
		private Label _accel_widget;
		private string _label;
		private string _accel;
		private bool _use_underline;
		private Gravity _gravity;

		private List<weak Label> children;
		private HashTable<weak Widget, ChildPropBag*> props;

		public override void style_set(Style? old_style) {
			foreach(weak Label child in children) {
				child.style = style;
			}
		}
		public override void forall(Gtk.Callback callback, void* data) {
			bool include_internals = false;

			if(include_internals) {
			}
			weak List<weak Label> iter = children;
			while(iter != null) {
				weak Widget child = iter.data;
				iter = iter.next;
				callback(child);
			}
		}
		public override void size_request(out Requisition r) {
			Requisition cr;
			r.width = 0;
			r.height = 0;
			foreach(Widget child in children) {
				if(!child.visible) continue;
				child.size_request(out cr);
				int padding = 0;
				child_get(child, "padding", &padding, null);
				switch(gravity) {
					case Gravity.LEFT:
					case Gravity.RIGHT:
						r.width = r.width>cr.width?r.width:cr.width;
						r.height += (cr.height + padding);
					break;
					case Gravity.UP:
					case Gravity.DOWN:
						r.height = r.height>cr.height?r.height:cr.height;
						r.width += (cr.width + padding);
					break;
				}
			}
		}
		public override void size_allocate(Gdk.Rectangle a) {
			allocation = (Allocation)a;
			Requisition cr;
			Gdk.Rectangle ca = {0, 0, 0, 0};
			int x = a.x;
			int y = a.y;
			int num_vis = 0;
			int expand = 0;
			foreach(Widget child in children) {
				if(!child.visible) continue;
				num_vis++;
			}
			switch(gravity) {
				case Gravity.LEFT:
				case Gravity.RIGHT:
					expand = allocation.height - requisition.height;
					break;
				case Gravity.UP:
				case Gravity.DOWN:
					expand = allocation.width - requisition.width;
					break;
			}
			if(expand < 0) expand = 0;
			foreach(Widget child in children) {
				if(!child.visible) continue;
				child.get_child_requisition(out cr);
				int padding = 0;
				child_get(child, "padding", &padding, null);
				Pango.Alignment alignment = Pango.Alignment.LEFT;
				child_get(child, "alignment", &alignment, null);
				switch(gravity) {
					case Gravity.LEFT:
					case Gravity.RIGHT:
						ca.x = x;
						ca.y = y;
						ca.width = a.width;
						ca.height = cr.height + expand/num_vis;
						y += (ca.height + padding);
						switch(alignment) {
							case Pango.Alignment.LEFT:
							break;	
							case Pango.Alignment.RIGHT:
								ca.y += padding;
							break;	
							case Pango.Alignment.CENTER:
								ca.y += padding/2;
							break;
						}
					break;
					case Gravity.UP:
					case Gravity.DOWN:
						ca.x = x;
						ca.y = y;
						ca.width = cr.width + expand/num_vis;
						ca.height = a.height;
						x += (ca.width + padding);
						switch(alignment) {
							case Pango.Alignment.LEFT:
							break;	
							case Pango.Alignment.RIGHT:
								ca.x += padding;
							break;	
							case Pango.Alignment.CENTER:
								ca.x += padding/2;
							break;
						}
					break;
				}
				child.size_allocate(ca);
			}
		}
		public override void get_child_property(Gtk.Widget child, uint id,
				Value value, ParamSpec pspec) {
			switch(id) {
				case PROP_ALIGNMENT:
					ChildPropBag* prop = props.lookup(child);
					assert(prop != null);
					value.set_enum(prop->alignment);
				break;
				case PROP_PADDING:
					ChildPropBag* prop = props.lookup(child);
					assert(prop != null);
					value.set_int(prop->padding);
				break;
			}
		}
		public override void set_child_property(Gtk.Widget child, uint id,
				Value value, ParamSpec pspec) {
			switch(id) {
				case PROP_ALIGNMENT:
					Pango.Alignment alignment = (Pango.Alignment)value.get_enum();
					ChildPropBag* prop = props.lookup(child);
					assert(prop != null);
					if(prop->alignment != alignment) {
						prop->alignment = alignment;
						update_label_gravity(child as Label);
					}
				break;
				case PROP_PADDING:
					int padding = value.get_int();
					ChildPropBag* prop = props.lookup(child);
					assert(prop != null);
					if(prop->padding != padding) {
						prop->padding = padding;
						queue_resize();
					}
				break;
			}
		
		}
		private void update_label_gravity(Label child) {
			double text_angle = gravity_to_text_angle(gravity);
			Pango.Alignment alignment = Pango.Alignment.LEFT;
		   	child_get(child, "alignment", &alignment, null);
			double al = 0.0;
			switch(alignment) {
				case Pango.Alignment.LEFT:
					al = 0.0;
				break;
				case Pango.Alignment.CENTER:
					al = 0.5;
				break;
				case Pango.Alignment.RIGHT:
					al = 1.0;
				break;
			}
			switch(_gravity) {
				case Gravity.DOWN:
				case Gravity.UP:
					child.set_alignment( (float)al, (float) 0.5);
				break;
				case Gravity.LEFT:
				case Gravity.RIGHT:
					child.set_alignment( (float)0.5, (float)al);
				break;
			}
			child.angle = text_angle;
		}
	}

}
