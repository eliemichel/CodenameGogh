#include "mainGui.h"

#include "Ui/UiApp.h"
#include "Ui/UiWindow.h"
#include "Ui/MainLayout.h"
#include "Ui/UiBase.h"
#include "Parameter.h"

#include "Logger.h"
#include "Ui/NodeArea.h"
#include "Ui/ParameterWidget.h"
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
	// TEST
	Variant var;
	var = "42";
	DEBUG_LOG << var.toInt();

	UiApp app;
	UiWindow window(1200, 600, "Gogh");

	MainLayout *popupLayout = new MainLayout();

	UiHBoxLayout *mainLayout = new UiHBoxLayout();

	// Node Area
	NodeArea *nodeArea = new NodeArea();
	mainLayout->AddItem(nodeArea);

	UiContextMenu *contextMenu = new UiContextMenu(popupLayout);
	contextMenu->SetItemLabels({ "Lorem", "Ipsum", "Dolor", "Sit", "Amet" });
	nodeArea->SetContextMenu(contextMenu);

	// Sidebar
	UiVBoxLayout *layout = new UiVBoxLayout();

	{
		Parameter *param = new Parameter();
		param->setType(StringType);
		param->setName("Yo");
		param->set("bloum");
		ParameterWidget *paramWidget = new ParameterWidget(popupLayout);
		paramWidget->SetParameter(param);
		paramWidget->SetInnerSizeHint(0, 0, 0, 30);
		layout->AddItem(paramWidget);
	}

	{
		Parameter *param = new Parameter();
		param->setType(IntType);
		param->setName("An int param");
		param->set(28);
		ParameterWidget *paramWidget = new ParameterWidget(popupLayout);
		paramWidget->SetParameter(param);
		paramWidget->SetInnerSizeHint(0, 0, 0, 30);
		layout->AddItem(paramWidget);
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
		ParameterWidget *paramWidget = new ParameterWidget(popupLayout);
		paramWidget->SetParameter(param);
		paramWidget->SetInnerSizeHint(0, 0, 0, 30);
		layout->AddItem(paramWidget);
	}

	{
		Parameter *param = new Parameter();
		param->setType(ButtonType);
		param->setName("Push me!");
		ParameterWidget *paramWidget = new ParameterWidget(popupLayout);
		paramWidget->SetParameter(param);
		paramWidget->SetInnerSizeHint(0, 0, 0, 30);
		layout->AddItem(paramWidget);
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

	while (!window.ShouldClose())
	{
		window.Render();
		window.Poll();
	}

	return EXIT_SUCCESS;
}
