#ifndef __PLASMA_HELLO_H__
#define __PLASMA_HELLO_H__  1

#include <KIcon>
#include <Plasma/Applet>
#include <Plasma/Svg>

#include <kwindowsystem.h>


class QSizeF;
class QStackedWidget;
class QGraphicsProxyWidget;
class AppModel;
class QMenuBar;

class PlasmaGlobalMenu: public Plasma::Applet
{
	Q_OBJECT
	public:
		PlasmaGlobalMenu(QObject *parent, const QVariantList &args);
		~PlasmaGlobalMenu();

		void init();

	private:
		QGraphicsProxyWidget *_widget;
		KWindowSystem *_windowManager;
		QStackedWidget *_stackedWidget;
		QHash<WId, QMenuBar*> _menubarHash;
		QHash<WId, AppModel*> _modelHash;
		QWidget *_defaultWidget;

		QWidget* createDefaultMenu();
	private slots:
		void onActiveWindowChanged(WId xid);
};

K_EXPORT_PLASMA_APPLET(globalmenu, PlasmaGlobalMenu)

#endif /*__PLASMA_HELLO_H__ */
