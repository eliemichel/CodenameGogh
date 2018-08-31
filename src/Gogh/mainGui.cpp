#include "mainGui.h"

#include "Ui/UiApp.h"
#include "Ui/UiWindow.h"
#include "Ui/MainLayout.h"
#include "Ui/UiBase.h"
#include "Parameter.h"

#include "Logger.h"
#include "Node.h"
#include "Graph.h"
#include "Ui/NodeArea.h"
#include "Ui/ParameterDelegate.h"
#include "Ui/UiTextInput.h"
#include "Ui/UiIntInput.h"
#include "Ui/UiEnumInput.h"
#include "Ui/UiButton.h"
#include "Ui/UiContextMenu.h"
#include "Variant.h"

#include <GLFW/glfw3.h>
#include <nanovg.h>

#include <algorithm>
#include <cassert>

int mainGui(const ArgParse & args)
{
	Parameter *pParam;
	Node *pNode;
	Graph *graph = new Graph();
	{
		Node *node = new Node();
		node->insertInputSlots(0, 1);
		node->insertOutputSlots(0, 2);
		node->insertParams(0, 1);
		Parameter & param = node->param(0);
		param.setType(EnumType);
		param.setName("Enum");
		param.insertMenuItems(0, 2);
		param.setMenuLabel(0, "Choice A");
		param.setMenuLabel(1, "Choice B");
		param.setMenuLabel(2, "Choice C");
		param.set(1);
		Parameter & param2 = node->param(1);
		param2.setType(StringType);
		param2.setName("Yo");
		param2.set("bloum");
		pParam = &param2;
		graph->addNode(node);

		pNode = node;
	}

	UiApp app;
	UiWindow window(1200, 600, "Gogh");

	MainLayout *popupLayout = new MainLayout();

	UiHBoxLayout *mainLayout = new UiHBoxLayout();

	// Node Area
	NodeArea *nodeArea = new NodeArea(graph, popupLayout);
	mainLayout->AddItem(nodeArea);

	UiContextMenu *contextMenu = new UiContextMenu(popupLayout);
	contextMenu->SetItemLabels({ "Lorem", "Ipsum", "Dolor", "Sit", "Amet" });
	nodeArea->SetContextMenu(contextMenu);

	// Sidebar
	UiVBoxLayout *layout = new UiVBoxLayout();

	{
		ParameterDelegate *paramDelegate = new ParameterDelegate(popupLayout);
		paramDelegate->SetParameter(pParam);
		paramDelegate->SetInnerSizeHint(0, 0, 0, 30);
		layout->AddItem(paramDelegate);
	}

	{
		Parameter *param = new Parameter();
		param->setType(StringType);
		param->setName("Yo");
		param->set("bloum");
		ParameterDelegate *paramDelegate = new ParameterDelegate(popupLayout);
		paramDelegate->SetParameter(param);
		paramDelegate->SetInnerSizeHint(0, 0, 0, 30);
		layout->AddItem(paramDelegate);
	}
	
	{
		Parameter *param = new Parameter();
		param->setType(IntType);
		param->setName("An int param");
		param->set(28);
		ParameterDelegate *paramDelegate = new ParameterDelegate(popupLayout);
		paramDelegate->SetParameter(param);
		paramDelegate->SetInnerSizeHint(0, 0, 0, 30);
		layout->AddItem(paramDelegate);
	}

	{
		Parameter *param = new Parameter();
		param->setType(EnumType);
		param->setName("Enum");
		param->insertMenuItems(0, 2);
		param->setMenuLabel(0, "Choice A");
		param->setMenuLabel(1, "Choice B");
		param->setMenuLabel(2, "Choice C");
		param->set(1);
		ParameterDelegate *paramDelegate = new ParameterDelegate(popupLayout);
		paramDelegate->SetParameter(param);
		paramDelegate->SetInnerSizeHint(0, 0, 0, 30);
		layout->AddItem(paramDelegate);
	}

	{
		Parameter *param = new Parameter();
		param->setType(ButtonType);
		param->setName("Push me!");
		ParameterDelegate *paramDelegate = new ParameterDelegate(popupLayout);
		paramDelegate->SetParameter(param);
		paramDelegate->SetInnerSizeHint(0, 0, 0, 30);
		layout->AddItem(paramDelegate);
	}

	UiButton *button = new UiButton();
	button->SetText("Test 1");
	button->SetInnerSizeHint(0, 0, 0, 30);
	layout->AddItem(button);

	button = new UiButton();
	button->SetText("Test 2");
	button->SetInnerSizeHint(0, 0, 0, 30);
	layout->AddItem(button);

	UiTextInput *textInput = new UiTextInput();
	textInput->SetText("blabla");
	textInput->SetInnerSizeHint(0, 0, 0, 30);
	layout->AddItem(textInput);

	UiIntInput *intInput = new UiIntInput();
	intInput->SetValue(42);
	intInput->SetInnerSizeHint(0, 0, 0, 30);
	layout->AddItem(intInput);

	UiEnumInput *enumInput = new UiEnumInput(popupLayout);
	enumInput->SetItemLabels({"Choice A", "Choice B"});
	enumInput->SetInnerSizeHint(0, 0, 0, 30);
	layout->AddItem(enumInput);

	layout->SetInnerSizeHint(0, 0, 300, 0);
	mainLayout->AddItem(layout);

	popupLayout->AddItem(mainLayout);
	window.SetContent(popupLayout);
	popupLayout->SetRect(0, 0, 1200, 600);

	pNode->insertParams(0, 0);
	{
		Parameter & param = pNode->param(0);
		param.setType(IntType);
		param.setName("Stream #");
		param.set(3);
	}
	pNode->removeParams(1, 1);

	while (!window.ShouldClose())
	{
		window.Render();
		window.Poll();
	}

	delete graph;

	return EXIT_SUCCESS;
}
