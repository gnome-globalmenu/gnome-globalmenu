internal class Patcher {
	MenuBar menubar;
	Widget widget;
	public Patcher() {
		menubar = new MenuBar();
		widget = new Widget();
	}
	~Patcher() {
		Superrider.release_all();
	}
}
