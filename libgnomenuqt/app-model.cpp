/* vi: set sw=4 ts=4: */
/*
 * app-model.c
 *
 * This file is part of ________.
 *
 * Copyright (C) 2008 - Mingxi Wu <fengshenx@gmail.com>.
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

#include "app-model.h"

#include <limits.h>

#include <QHash>
#include <QString>
#include <QMenuBar>
#include <QDebug>
#include <QtGlobal>
#include <QByteArray>
#include <QX11Info>
#include <QXmlStreamReader>
#include <QMenu>
#include <QAction>

#include <X11/Xlib.h>
#include <X11/Xatom.h>


class AppNode {
public:
	AppNode(AppNode *parent = NULL) 
		:_parent(parent)
	{
		if (_parent)
			_parent->addChild(this);
	}

	~AppNode() 
	{
		foreach(AppNode *child, _children) {
			delete child;
		}
	}

	enum NodeType {
		NodeMenu = 0,
		NodeItem
	};

	inline AppNode *parent() { return _parent;}
	inline const QList<AppNode*>& children(){ return _children;}
	inline const QHash<QString, QString>& properties() { return _properties;}

	inline void addProperty(const QString &name, const QString &value)
	{
		_properties.insert(name, value);
	}

	inline bool hasProperty(const QString &name) {
		return _properties.contains(name);
	}

	inline QString getProperty(const QString &name) 
	{
		return _properties.value(name);
	}

	inline void addChild(AppNode *child) 
	{
		_children.push_back(child);
	}

	static void printNode(AppNode *model, int indent)
	{
		foreach(AppNode *m, model->children())
		{
			QString line;
			for (int i=0 ; i<indent; i++)
				line.append(' ');
			line.append(m->properties().value("label"));
			qDebug() << indent << line;

			printNode(m, indent+1);
		}
	}

private:
	QHash<QString, QString> _properties;
	AppNode *_parent;
	QList<AppNode*> _children;
	NodeType _type;
};

AppModel::AppModel(WId xid)
	:_menubar(NULL),
	_xid(xid)
{
	_rootNode = new AppNode();

	updateContent();
	AppNode::printNode(_rootNode, 0);
}

AppModel::~AppModel()
{

}


void AppModel::updateContent()
{
	QByteArray menuContent;
	Atom atomContext;
	Atom atomString;
	Atom type;
	int e;
	int format;
	unsigned long nitems, after;
	unsigned char *data = NULL;
	Display *display = QX11Info::display();

	atomContext = XInternAtom(display, "_NET_GLOBALMENU_MENU_CONTEXT", True);
	atomString = XInternAtom(display, "STRING", False);

	if (atomContext == None || atomString == None)
		qDebug() << "Can't get atom...";

	if( XGetWindowProperty(display, _xid, atomContext, 0, INT_MAX, False, atomString,
			&type, &format, &nitems, &after, &data) == Success && data != NULL) {
		_content = QString::fromUtf8((char*)data);
		parseContent();
	}
	else qDebug() << "failed to get window property...";
	
	if (data)
		XFree(data);
}

static inline bool isMenu(AppNode *node)
{
	return node->children().isEmpty() ? false : 
		node->children().first()->getProperty("tag") == "menu";
}

QMenu* AppModel::createMenu(QWidget *parent, AppNode *node)
{
	QString label;
	QMenu *menu;
	AppNode *menuItem;

#if 0
	if (!isMenu(node)) {
		qDebug() << __func__ << "Not a menu!!";
		return NULL;
	} 
#endif

	menu = new QMenu(parent);
	menuItem = node->children().first();
	foreach(AppNode *child, menuItem->children()) {
		if (isMenu(child)) {
			menu->addMenu(createMenu(menu, child));
		} else 
			menu->addAction(createAction(menu, child));
	}

	label = node->getProperty("label");
	label.replace(QString("_"), QString("&"));
	menu->setTitle(label);

	return menu;
}

static QIcon* iconFromGtkStock(QString stockName)
{
	return NULL;
}

QAction* AppModel::createAction(QWidget *parent, AppNode *node)
{
	QAction *a = NULL;
	QString label = node->getProperty("label");
	bool isShow = false;

	a = new QAction(parent);

	isShow = node->hasProperty("visible") ? (node->getProperty("visible") != "0"): true;

	if (node->getProperty("type") == "s" && isShow) {
		a->setSeparator(true);
		return a;
	}

	label.replace(QString("_"), QString("&"));
	a->setText(label);

	if (!isShow)
		a->setVisible(false);
	else 
		a->setVisible(true);

	// image item
	if (node->getProperty("type") == "i" && node->hasProperty("icon")) {
		QString iconStock = node->getProperty("icon");
		QIcon *icon = iconFromGtkStock(iconStock);
		if (icon) {
			a->setIcon(*icon);
			delete icon;
		} else 
			qDebug() << "no icon found";
	} else if (node->getProperty("type") == "c") {
		a->setChecked(true);
	} else if (node->getProperty("type") == "r") {
		qDebug() << __func__ << "FIXME: handle radio items...";
	}

	// FIXME : handle state and sensitive property...
	
	return a;
}

QMenuBar* AppModel::createMenuBar()
{
	AppNode *menuItem;
	if (_menubar)
		return _menubar;
	
	if (_rootNode->children().isEmpty())
		return NULL;

	_menubar = new QMenuBar(NULL);
	menuItem = _rootNode->children().first();
	foreach(AppNode *node, menuItem->children()) {
		qDebug() << "rootNode's" << node->getProperty("label");
		if (isMenu(node)) {
			_menubar->addMenu(createMenu(_menubar, node));
		} 
//		else _menubar->addAction(createAction(_menubar, node));
	}
	return _menubar;
}

void AppModel::parseContent()
{
	QXmlStreamReader xml;
	xml.addData(_content);

	AppNode *currentNode = _rootNode;
	while (!xml.atEnd()) {
		xml.readNext();
		if (xml.isStartElement()) {
			AppNode *newNode = new AppNode(currentNode);
			currentNode = newNode;
			newNode->addProperty("tag", xml.name().toString());
			if (xml.name().toString() == "item") {
#define ADD_IF_CONTAINS(attr) if (!xml.attributes().value(attr).isNull()) { \
					newNode->addProperty(attr, xml.attributes().value(attr).toString()); \
				}
				ADD_IF_CONTAINS("label");
				ADD_IF_CONTAINS("accel");
				ADD_IF_CONTAINS("type");
				ADD_IF_CONTAINS("icon");
				ADD_IF_CONTAINS("visible");
				ADD_IF_CONTAINS("sensitive");
				ADD_IF_CONTAINS("state");
#undef ADD_IF_CONTAINS
			}
		} else if (xml.isEndElement()) {
			currentNode = currentNode->parent();
		} else if (xml.isCharacters() && !xml.isWhitespace()) {
			qDebug() << "text" << xml.text().toString();
		}

		if (xml.hasError()) {
			qDebug() << "error!!" << xml.errorString();
			break;
		}
	}

}
