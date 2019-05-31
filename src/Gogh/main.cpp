#include <memory>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QUrl>

#include "Logger.h"
#include "Parameter.h"
#include "Graph.h"

using namespace Gogh;

int main(int argc, char *argv[])
{
	QGuiApplication app(argc, argv);

	QQmlApplicationEngine engine;
	engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
	if (engine.rootObjects().isEmpty())
	{
		return -1;
	}

	return app.exec();
}
