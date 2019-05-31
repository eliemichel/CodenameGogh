#include <memory>

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QUrl>

#include "Logger.h"
#include "Parameter.h"

struct Edge;
struct Node;

struct Slot {
	Slot() { DEBUG_LOG << "Slot ctor"; }
	~Slot() { DEBUG_LOG << "Slot dtor"; }

	std::weak_ptr<Edge> edge; // null iff the slot is not connected
	std::weak_ptr<Node> node;

	std::shared_ptr<Node> getNode() const noexcept {
		return node.lock();
	}

	bool isGarbage() const noexcept {
		return node.expired();
	}
};

struct Node: public std::enable_shared_from_this<Node> {
	Node() { DEBUG_LOG << "Node ctor"; }
	~Node() { DEBUG_LOG << "Node dtor"; }

	int payload;
	std::vector<std::unique_ptr<Gogh::Parameter>> parameters;
	std::vector<std::shared_ptr<Slot>> inputs;
	std::vector<std::shared_ptr<Slot>> outputs;

	std::shared_ptr<Slot> addInput() noexcept {
		std::shared_ptr<Slot> s = std::make_shared<Slot>();
		inputs.push_back(s);
		return s;
	}

	std::shared_ptr<Slot> addOutput() noexcept {
		std::shared_ptr<Slot> s = std::make_shared<Slot>();
		s->node = weak_from_this();
		outputs.push_back(s);
		return s;
	}

	std::shared_ptr<Slot> getInput(int index) noexcept {
		if (index < 0 || index >= inputs.size())
		{
			WARN_LOG << "Invalid input index: " << index << " (node has " << inputs.size() << " inputs)";
			return nullptr;
		}
		return inputs[index];
	}

	std::shared_ptr<Slot> getOutput(int index) noexcept {
		if (index < 0 || index >= outputs.size())
		{
			WARN_LOG << "Invalid output index: " << index << " (node has " << outputs.size() << " outputs)";
			return nullptr;
		}
		return outputs[index];
	}
};

struct Edge {
	Edge() { DEBUG_LOG << "Edge ctor"; }
	~Edge() { DEBUG_LOG << "Edge dtor"; }

	std::weak_ptr<Slot> origin;
	std::weak_ptr<Slot> destination;

	std::shared_ptr<Slot> getOrigin() const noexcept {
		return origin.lock();
	}
	std::shared_ptr<Slot> getDestination() const noexcept {
		return destination.lock();
	}

	bool isGarbage() const noexcept {
		return origin.expired() || destination.expired();
	}
};

struct Graph {
	Graph() { DEBUG_LOG << "Graph ctor"; }
	~Graph() { DEBUG_LOG << "Graph dtor"; }

	std::vector<std::shared_ptr<Node>> nodes;
	std::vector<std::shared_ptr<Edge>> edges;

	std::shared_ptr<Node> addNode() noexcept {
		std::shared_ptr<Node> n = std::make_shared<Node>();
		nodes.push_back(n);
		return n;
	}

	void removeNode(std::shared_ptr<Node> node) noexcept {
		if (!node) {
			WARN_LOG << "Removing null node";
			return;
		}

		// Remove input edges
		for (auto input : node->inputs)
		{
			if (auto edge = input->edge.lock()) {
				removeEdge(edge);
			}
		}

		// Remove output edges
		for (auto output : node->outputs)
		{
			if (auto edge = output->edge.lock()) {
				removeEdge(edge);
			}
		}

		// Remove node
		nodes.erase(std::find(nodes.begin(), nodes.end(), node));
	}

	std::shared_ptr<Edge> addEdge(std::shared_ptr<Slot> origin, std::shared_ptr<Slot> destination) noexcept {
		if (!origin)
		{
			WARN_LOG << "Null origin slot";
			return nullptr;
		}
		if (!destination)
		{
			WARN_LOG << "Null destination slot";
			return nullptr;
		}

		std::shared_ptr<Edge> e = std::make_shared<Edge>();
		e->origin = origin;
		origin->edge = e;
		e->destination = destination;
		destination->edge = e;
		edges.push_back(e);
		return e;
	}

	void removeEdge(std::shared_ptr<Edge> edge) noexcept {
		if (!edge) {
			WARN_LOG << "Removing null edge";
			return;
		}
		
		if (auto origin = edge->origin.lock())
		{
			origin->edge = std::weak_ptr<Edge>();
		}
		if (auto destination = edge->destination.lock())
		{
			destination->edge = std::weak_ptr<Edge>();
		}
		edges.erase(std::find(edges.begin(), edges.end(), edge));
	}
};

void test()
{
	Gogh::Parameter param;
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
