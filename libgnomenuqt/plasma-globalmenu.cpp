/* vi: set sw=4 ts=4: */
/*
 * plasma-globalmenu.cpp
 *
 * This file is part of ________.
 *
 * Copyright (C) 2008 - Mingxi Wu<fengshenx@gmail.com>.
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

#include <QApplication>
#include <QPainter>
#include <QFontMetrics>
#include <QSizeF>
#include <QMenuBar>
#include <QStackedWidget>
#include <QGraphicsProxyWidget>
#include <QLabel>

#include <KDebug>
#include <plasma/svg.h>
#include <plasma/theme.h>

#include <X11/Xlib.h>

#include "app-model.h"

PlasmaGlobalMenu::PlasmaGlobalMenu(QObject *parent, const QVariantList &args)
	:Plasma::Applet(parent, args)
{
	setBackgroundHints(DefaultBackground);
	resize(200, 200);

	_windowManager= KWindowSystem::self();

	connect(_windowManager, SIGNAL(activeWindowChanged(WId)), 
				this, SLOT(onActiveWindowChanged(WId)));
	
}

PlasmaGlobalMenu::~PlasmaGlobalMenu()
{
	if (hasFailedToLaunch()) {

	} else {

	}
}

void PlasmaGlobalMenu::init()
{
	_stackedWidget = new QStackedWidget;
	_defaultWidget = createDefaultMenu();
	_stackedWidget->addWidget(_defaultWidget);

	_widget = new QGraphicsProxyWidget(this);
	_widget->setWidget(_stackedWidget);
}

void PlasmaGlobalMenu::onActiveWindowChanged(WId xid)
{
	AppModel *model;
	qDebug() << "window changed" << QString().sprintf("0x%x", xid);

	QWidget *self = QApplication::activeWindow();

	if (self && self->winId() == xid)
		return;

	if (!_modelHash.contains(xid)) {
		QMenuBar *menubar;

		model = new AppModel(xid);

		if (model->isNull()) {
			delete model;
			return;
		}

		menubar = model->createMenuBar();
		_stackedWidget->addWidget(menubar);
		_modelHash.insert(xid, model);
		_menubarHash.insert(xid, menubar);
	} 
	_stackedWidget->setCurrentWidget(_menubarHash.value(xid));
}

QWidget* PlasmaGlobalMenu::createDefaultMenu()
{
	return new QLabel("hahaha");
}

#include "plasma-globalmenu.moc"
