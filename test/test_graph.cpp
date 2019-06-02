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
