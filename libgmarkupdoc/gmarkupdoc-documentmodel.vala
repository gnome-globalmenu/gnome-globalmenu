using GLib;
using GLibCompat;

[CCode (cprefix = "GMarkup", lower_case_cprefix = "g_markup_")]
namespace GMarkup {
	/**
	 * The document model for GMarkup. It is some kind of different from the DOM model.
	 */
	public interface DocumentModel: GLib.Object {
	}
}
