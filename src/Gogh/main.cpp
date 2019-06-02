/**
* This file is part of Gogh.
* It is released under the terms of the MIT License:
*
* Copyright (c) 2018 - 2019 -- Élie Michel <elie.michel@exppad.com>
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the “Software”), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* The Software is provided “as is”, without warranty of any kind, express or
* implied, including but not limited to the warranties of merchantability,
* fitness for a particular purpose and non-infringement. In no event shall the
* authors or copyright holders be liable for any claim, damages or other
* liability, whether in an action of contract, tort or otherwise, arising
* from, out of or in connection with the software or the use or other dealings
* in the Software.
*/

#include <memory>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QUrl>
#include <QApplication>

#include "Logger.h"
#include "Parameter.h"
#include "Graph.h"
#include "Gui/MainWindow.h"

int main(int argc, char *argv[])
{
	/*
	QGuiApplication app(argc, argv);

	QQmlApplicationEngine engine;
	engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
	if (engine.rootObjects().isEmpty())
	{
		return -1;
	}
	return app.exec();
	/*/
	QApplication app(argc, argv);

	Gogh::Gui::MainWindow window;
	window.resize(800, 600);
	window.show();

	return app.exec();
	//*/
}
