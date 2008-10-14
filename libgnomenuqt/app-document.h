/* vi: set sw=4 ts=4: */
/*
 * app-document.h
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

#ifndef __APP_DOCUMENT_H__
#define __APP_DOCUMENT_H__  1

#include <QObject>
#include <QXmlStreamReader>
#include <QDBusConnection>

class AppModel;

class QMenuBar;
class QMenu;
class QAction;

class AppDocument : public QObject {
	Q_OBJECT

public:
	AppDocument(QString service, QString path);
	~AppDocument() {}
	
	QMenuBar *createMenuBar();
private:
	QString _service;
	QString _path;
	QString _xid;
	QString _name;
	QMenuBar *_menubar;

	void parseDocument();
	AppModel *_model;


	QAction *createAction(QWidget *parent, AppModel *model);
	QMenu* createMenu(QWidget *parent, AppModel *model);

private slots:
	void onNameOwnerChanged(QString bus, QString oldOwner, QString newOwner);
	void onInserted(QString pathName, QString nodeName, int pos);
	void onRemoved(QString parentName, QString nodeName);
	void onUpdated(QString nodeName, QString propName);
};

#endif /*__APP_DOCUMENT_H__ */
