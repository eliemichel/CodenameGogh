#include <memory>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QUrl>

#include "Logger.h"
#include "Parameter.h"
#include "Graph.h"

using namespace Gogh;

void test()
{
	Parameter param;
	param.setName("p");

	Graph g;
	{
		std::shared_ptr<Node> n0 = g.addNode();
		n0->payload = 18;
		std::shared_ptr<Slot> s0 = n0->addOutput();
		std::shared_ptr<Node> n1 = g.addNode();
		n1->payload = 27;
		std::shared_ptr<Slot> s1 = n1->addInput();
		std::shared_ptr<Edge> e = g.addEdge(s0, s1);
	}

	LOG << "removing node n0...";
	g.removeNode(g.nodes[0]);
	LOG << "done.";

	std::shared_ptr<Node> n1 = g.nodes[0];
	std::shared_ptr<Slot> is00 = n1->getInput(0);
	if (std::shared_ptr<Edge> edge = is00->edge.lock())
	{
		std::shared_ptr<Slot> s = edge->getOrigin();
		std::shared_ptr<Node> np = s->getNode();
		LOG << "n1 is connected to [" << np->payload << "]";
	}
	else {
		LOG << "n1 is not connected";
	}
}

int main(int argc, char *argv[])
{
	test();

	QGuiApplication app(argc, argv);

	QQmlApplicationEngine engine;
	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
	if (engine.rootObjects().isEmpty())
	{
		return -1;
	}

	return app.exec();
}
