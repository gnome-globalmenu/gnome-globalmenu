using Gtk;

namespace Gnomenu {

	public class MenuLabel: Gtk.Container {
		private struct ChildPropBag {
			public Pango.Alignment alignment;
		}
		static const int PROP_ALIGNMENT = 1234;
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
		}
		public string accel {
			get {
				return _accel_widget.label;
			}
			set {
				_accel_widget.label = value;
			}
		}

		public string label {
			get {
				return _label_widget.label;
			}
			set {
				_label_widget.label = value;
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
		construct {
			props = new HashTable<weak Widget, ChildPropBag*>.full(direct_hash, direct_equal,
				null, free);
			set_flags(WidgetFlags.NO_WINDOW);
			_accel_widget = new Label("");
			_accel_widget.visible = true;
			_label_widget = new Label("");
			_label_widget.visible = true;
			add(_label_widget);
			add(_accel_widget);
			child_set(_accel_widget, "alignment", Pango.Alignment.RIGHT, null);
		}
		public override void add(Widget child) {
			if(!(child is Label)) {
				warning("only GtkLabel is accepted");
				return;
			}
			children.append(child as Label);
			child.set_parent(this);
			props.insert(child, (ChildPropBag*)malloc(sizeof(ChildPropBag)));
			update_label_gravity(child as Label);
		}
		public override void remove(Widget child) {
			children.remove_all(child as Label);
			child.unparent();
			props.remove(child);
		}
		private override void realize() {
			base.realize();
		}
		private override void map() {
			base.map();
		}
		private Label _label_widget;
		private Label _accel_widget;
		private Gravity _gravity;

		private List<weak Label> children;
		private HashTable<weak Widget, ChildPropBag*> props;

		private override void forall(Gtk.Callback callback, void* data) {
			bool include_internals = false;

			if(include_internals) {
			}
			foreach(Widget child in children) {
				callback(child);
			}
		}
		private override void size_request(out Requisition r) {
			Requisition cr;
			r.width = 0;
			r.height = 0;
			foreach(Widget child in children) {
				if(!child.visible) continue;
				child.size_request(out cr);
				switch(gravity) {
					case Gravity.LEFT:
					case Gravity.RIGHT:
						r.width = r.width>cr.width?r.width:cr.width;
						r.height += cr.height;
					break;
					case Gravity.UP:
					case Gravity.DOWN:
						r.height = r.height>cr.height?r.height:cr.height;
						r.width += cr.width;
					break;
				}
			}
			message("%d %d", r.width, r.height);
		}
		private override void size_allocate(Gdk.Rectangle a) {
			allocation = (Allocation)a;
			Requisition cr;
			Gdk.Rectangle ca;
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
				switch(gravity) {
					case Gravity.LEFT:
					case Gravity.RIGHT:
						ca.x = x;
						ca.y = y;
						ca.width = a.width;
						ca.height = cr.height + expand/num_vis;
						y += ca.height;
					break;
					case Gravity.UP:
					case Gravity.DOWN:
						ca.x = x;
						ca.y = y;
						ca.width = cr.width + expand/num_vis;
						ca.height = a.height;
						x += ca.width;
					break;
				}
				child.size_allocate(ca);
			}
		}
		private override void get_child_property(Gtk.Widget child, uint id,
				Value value, ParamSpec pspec) {
			switch(id) {
				case PROP_ALIGNMENT:
					ChildPropBag* prop = props.lookup(child);
					assert(prop != null);
					value.set_enum(prop->alignment);
				break;
			}
		}
		private override void set_child_property(Gtk.Widget child, uint id,
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
			}
		
		}
		private void update_label_gravity(Label child) {
			double text_angle = gravity_to_text_angle(gravity);
			Pango.Alignment alignment;
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
