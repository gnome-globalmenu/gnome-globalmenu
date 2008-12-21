/* vi: set sw=4 ts=4: */
/*
 * app-model.h
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

#ifndef __APP_MODEL_H__
#define __APP_MODEL_H__  1

#include <QObject>
#include <QWidget>

class QMenuBar;
class QMenu;
class QAction;

class AppNode;

class AppModel : public QObject
{
	Q_OBJECT
	
public:
	AppModel(WId wid);
	~AppModel();

	QMenuBar *createMenuBar();
	inline bool isNull() { return _content.isNull();}

private:
	QString _content;
	QMenuBar *_menubar;
	WId _xid;

	AppNode *_rootNode;
	

	void parseContent();
	void updateContent();
	QMenu *createMenu(QWidget *parent, AppNode *node);
	QAction *createAction(QWidget *parent, AppNode *node);
};

#endif /*__APP_MODEL_H__ */
