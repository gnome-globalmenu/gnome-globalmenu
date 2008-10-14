#ifndef __PLASMA_HELLO_H__
#define __PLASMA_HELLO_H__  1

#include <KIcon>
#include <Plasma/Applet>
#include <Plasma/Svg>
#include <QWebView>
#include <QVBoxLayout>
#include <QPushButton>
#include <QGraphicsProxyWidget>
#include <QFrame>

#include <kwindowsystem.h>

#include "remote-document.h"

class QSizeF;

class PlasmaGlobalMenu: public Plasma::Applet
{
	Q_OBJECT
	public:
		PlasmaGlobalMenu(QObject *parent, const QVariantList &args);
		~PlasmaGlobalMenu();

        void paintInterface(QPainter *painter,
                                    const QStyleOptionGraphicsItem *option,
                                    const QRect& contentsRect);
		void init();

	private:
		Plasma::Svg m_svg;
		KIcon m_icon;
		QVBoxLayout *m_vlayout;
		QWebView *m_webView;
		QPushButton  *m_button;
		QGraphicsProxyWidget *m_widget;
		QFrame *m_frame;
		KWindowSystem *m_windowManager;

		RemoteDocument *_rDoc;
	private slots:
		void onActiveWindowChanged(WId xid);
};

K_EXPORT_PLASMA_APPLET(globalmenu, PlasmaGlobalMenu)

#endif /*__PLASMA_HELLO_H__ */
