/* vi: set sw=4 ts=4: */
/*
 * server.h
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

#ifndef __SERVER_H__
#define __SERVER_H__  1

#include <QObject>
#include <QHash>

class QDBusInterface;
class AppDocument;

class Server : public QObject
{
	Q_OBJECT
public:
	Server(QString path, QString interface);
	~Server();

	AppDocument*  queryByXID(const QString &xid);
//	QList<QString> clientList();
//	QList<QString> nameList();
	bool isStarted();

signals:
	void newApp(QString name);

private:
	QString _service;
	QString _path;
	bool _started;
	QDBusInterface *_DBusInterface;
	void parseDocument();

	QHash<QString, AppDocument*> _DocumentHash;
//	QHash<QString, AppClient*> _ClientHash;
};

#endif /*__SERVER_H__ */
