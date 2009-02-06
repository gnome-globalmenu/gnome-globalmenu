[CCode (cprefix="X", cheader_filename = "X11/Xlib.h,X11/Xutil.h,gdk/gdkx.h", lower_case_cprefix = "X")]
namespace Xlib {

	[SimpleType]
	[CCode (cname = "Window")]
	public struct Window {
		[CCode (cname = "GDK_WINDOW_XWINDOW")]
		public static Window from_gdk(Gdk.Window window);
	}
	[Compact]
	[CCode (cname = "Display")]
	public class Display {
		[CCode (cname = "GDK_DISPLAY_XDISPLAY")]
		public static weak Display from_gdk(Gdk.Display display);
	}
	[SimpleType]
	public struct Atom {
		[CCode (cname = "gdk_x11_xatom_to_atom")]
		public Gdk.Atom to_gdk();
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
