/* vi: set sw=4 ts=4: */
/*
 * server.cpp
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

#include "server.h"

#include <QDebug>
#include <QtGlobal>
#include <QDBusInterface>
#include <QDBusReply>
#include <QXmlStreamReader>

#include "app-document.h"

Server::Server(QString service, QString path)
	:_service(service),
	_path(path),
	_started(false)
{
	_DBusInterface = new QDBusInterface(_service, _path, 
							  			"org.gnome.GlobalMenu.Document");

	parseDocument();
}

Server::~Server()
{
	if (_DBusInterface)
		delete _DBusInterface;

	foreach (AppDocument *doc, _DocumentHash.values()) {
		delete doc;
	}
}

bool Server::isStarted()
{

}

void Server::parseDocument()
{
	QDBusReply<QString> reply = _DBusInterface->call("QueryRoot", 0);
	if (reply.isValid()) {
		qDebug() << reply.value();
		qDebug() << "QueryRoot OK!";
	}
	else qDebug() << reply.error().message();

	QXmlStreamReader xml;
	xml.addData(reply);
	while (!xml.atEnd()) {
		xml.readNext();
		if (xml.isStartElement()) {
			if (xml.name() == "client") {
				QString service = xml.attributes().value("bus").toString();
				QString xid = xml.attributes().value("name").toString();
				AppDocument *doc = new AppDocument(service,
						"/org/gnome/GlobalMenu/Application");

				qDebug() << __func__ << "xid = " << xid;
				_DocumentHash.insert(xid, doc);
				// TODO: create client here.
			}

		} else if (xml.isEndElement()) {
			qDebug() << "end elemnt" << xml.name().toString();
		} else if (xml.isCharacters() && !xml.isWhitespace()) {
			qDebug() << "text" << xml.text().toString();
		}
		if (xml.hasError()) {
			qDebug() << xml.errorString();
			break;
		}
	}
}

AppDocument* Server::queryByXID(const QString &xid)
{
	return _DocumentHash.value(xid);
}

#include "server.moc"
