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
		NodePtr n0 = g.addNode();
		n0->payload = 18;
		NodeOutputPtr s0 = n0->addOutput();
		NodePtr n1 = g.addNode();
		n1->payload = 27;
		NodeInputPtr s1 = n1->addInput();
		EdgePtr e = g.addEdge(s0, s1);
	}
	REQUIRE(g.nodes.size() == 2);
	REQUIRE(g.nodes[0]->outputs.size() == 1);
	REQUIRE(g.edges.size() == 1);

	NodePtr n1 = g.nodes[1];
	NodeInputPtr slot = n1->getInput(0);
	EdgePtr edge = slot->edge.lock();
	REQUIRE(edge);
	NodeOutputPtr s = edge->getOrigin();
	NodePtr np = s->getNode();
	REQUIRE(np->payload == 18);
}

TEST_CASE("Graph nodes deletion", "[graph]") {
	LOG << "TEST_CASE Graph nodes deletion";

	Graph g;
	{
		NodePtr n0 = g.addNode();
		n0->payload = 18;
		NodeOutputPtr s0 = n0->addOutput();
		NodePtr n1 = g.addNode();
		n1->payload = 27;
		NodeInputPtr s1 = n1->addInput();
		EdgePtr e = g.addEdge(s0, s1);
	}
	REQUIRE(g.nodes.size() == 2);
	REQUIRE(g.nodes[0]->outputs.size() == 1);
	REQUIRE(g.edges.size() == 1);

	LOG << "Removing node...";
	g.removeNode(g.nodes[0]);
	LOG << "Done.";

	REQUIRE(g.nodes.size() == 1);
	REQUIRE(g.edges.size() == 0);

	NodePtr n1 = g.nodes[0];
	NodeInputPtr is00 = n1->getInput(0);
	EdgePtr edge = is00->edge.lock();
	REQUIRE(!edge);
}

TEST_CASE("Graph node parameters", "[graph]") {
	LOG << "TEST_CASE Graph node parameters";

	Graph g;
	NodePtr n = g.addNode();

	n->parameters.push_back(std::make_unique<Parameter>());

	REQUIRE(n->parameters.size() == 1);
}
