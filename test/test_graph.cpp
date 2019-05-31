#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <memory>

#include "Logger.h"
#include "Parameter.h"
#include "Graph.h"

using namespace Gogh;

TEST_CASE("Graph nodes connection", "[graph]") {
	LOG << "TEST_CASE Graph nodes connection";

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
	REQUIRE(g.nodes.size() == 2);
	REQUIRE(g.nodes[0]->outputs.size() == 1);
	REQUIRE(g.edges.size() == 1);

	std::shared_ptr<Node> n1 = g.nodes[1];
	std::shared_ptr<Slot> slot = n1->getInput(0);
	std::shared_ptr<Edge> edge = slot->edge.lock();
	REQUIRE(edge);
	std::shared_ptr<Slot> s = edge->getOrigin();
	std::shared_ptr<Node> np = s->getNode();
	REQUIRE(np->payload == 18);
}

TEST_CASE("Graph nodes deletion", "[graph]") {
	LOG << "TEST_CASE Graph nodes deletion";

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
	REQUIRE(g.nodes.size() == 2);
	REQUIRE(g.nodes[0]->outputs.size() == 1);
	REQUIRE(g.edges.size() == 1);

	LOG << "Removing node...";
	g.removeNode(g.nodes[0]);
	LOG << "Done.";

	REQUIRE(g.nodes.size() == 1);
	REQUIRE(g.edges.size() == 0);

	std::shared_ptr<Node> n1 = g.nodes[0];
	std::shared_ptr<Slot> is00 = n1->getInput(0);
	std::shared_ptr<Edge> edge = is00->edge.lock();
	REQUIRE(!edge);
}

TEST_CASE("Graph node parameters", "[graph]") {
	LOG << "TEST_CASE Graph node parameters";

	Graph g;
	std::shared_ptr<Node> n = g.addNode();

	n->parameters.push_back(std::make_unique<Parameter>());

	REQUIRE(n->parameters.size() == 1);
}
