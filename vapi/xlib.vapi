[CCode (cprefix="X", cheader_filename = "X11/Xlib.h,X11/Xutil.h,gdk/gdkx.h", lower_case_cprefix = "X")]
namespace Xlib {

	[SimpleType]
	[CCode (cname = "Window")]
	public struct Window {
	}
	[SimpleType]
	[CCode (cname = "Display*")]
	public struct Display {
	}
	[SimpleType]
	[CCode (cname = "Atom")]
	public struct Atom {
	}

	[Compact]
	public class AnyEvent {
		public EventType type;
		public ulong serial;
		public bool send_event;
		public unowned Display display;
		public Window window;
	}

	public class PropertyEvent:AnyEvent {
		public Atom atom;
		public uint32 time;
		public int state;
	}
	[CCode (cprefix = "")]
	public enum EventType {
		PropertyNotify,
		KeyPress
	}
	[CCode (cprefix = "")]
	public enum EventMask {
		KeyPressMask,
	}
	public int SendEvent(Display display, Window window,
			bool propagate, long event_mask, AnyEvent event);
}
