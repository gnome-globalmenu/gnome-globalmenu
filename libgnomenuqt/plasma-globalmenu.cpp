/* vi: set sw=4 ts=4: */
/*
 * plasma-globalmenu.cpp
 *
 * This file is part of ________.
 *
 * Copyright (C) 2008 - ubuntu <ubuntu@gmail.com>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 * */

#include "plasma-globalmenu.h"
#include <QPainter>
#include <QFontMetrics>
#include <QSizeF>
#include <QMenuBar>

#include <KDebug>
#include <plasma/svg.h>
#include <plasma/theme.h>

#include "app-document.h"
#include "server.h"


Server *server;

PlasmaGlobalMenu::PlasmaGlobalMenu(QObject *parent, const QVariantList &args)
	:Plasma::Applet(parent, args),
	 m_svg(this),
	 m_icon("document")
{
	m_svg.setImagePath("widgets/background");
	setBackgroundHints(DefaultBackground);
	resize(200, 200);

	m_windowManager= KWindowSystem::self();

	connect(m_windowManager, SIGNAL(activeWindowChanged(WId)), 
				this, SLOT(onActiveWindowChanged(WId)));
	
	server = new Server("org.gnome.GlobalMenu.Server",
						"/org/gnome/GlobalMenu/Server");

	//delete server;
}

PlasmaGlobalMenu::~PlasmaGlobalMenu()
{
	if (hasFailedToLaunch()) {

	} else {

	}
}

void PlasmaGlobalMenu::init()
{
	if (m_icon.isNull()) {
		setFailedToLaunch(true, "No world to say globalmenu");
	}
	m_frame = new QFrame();
	m_vlayout = new QVBoxLayout();
	m_button  = new QPushButton("haha");
	m_webView = new QWebView();

	QMenuBar *menubar;

	menubar = server->queryByXID("71303200")->createMenuBar();
	m_vlayout->addWidget(menubar);
	m_vlayout->addWidget(m_button);
	m_vlayout->addWidget(m_webView);

	m_frame->setLayout(m_vlayout);

	m_widget = new QGraphicsProxyWidget(this);
	m_widget->setWidget(m_frame);

	m_webView->load(QUrl("file:///usr/share/ubuntu-artwork/home/index.html"));
	m_webView->show();
}

void PlasmaGlobalMenu::paintInterface(QPainter *p,
		const QStyleOptionGraphicsItem *option,
		const QRect &contentsRect)
{
	p->setRenderHint(QPainter::SmoothPixmapTransform);
	p->setRenderHint(QPainter::Antialiasing);

	m_svg.resize((int)contentsRect.width(), (int)contentsRect.height());
	m_svg.paint(p, (int)contentsRect.left(), (int)contentsRect.top());

	p->drawPixmap(7, 0, m_icon.pixmap((int)contentsRect.width(), (int)contentsRect.width()-14));
	p->save();
	p->setPen(Qt::white);
	p->drawText(contentsRect,
				Qt::AlignBottom | Qt::AlignHCenter,
				"GlobalMenu Plasmoid!");
	p->restore();
}

void PlasmaGlobalMenu::onActiveWindowChanged(WId xid)
{
	qDebug() << "window changed" << xid;
}


#include "plasma-globalmenu.moc"
