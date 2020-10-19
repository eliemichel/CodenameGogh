#include <iostream>

#include <QApplication>
#include <QFile>

#include <nodes/FlowViewStyle>
#include <nodes/ConnectionStyle>
#include <nodes/Connection>

#include "MainWindow.h"
#include "MainWindowStyle.h"
#include "dialogs/WelcomeDialog.h"
#include "widgets/AddNodeMenu.h"

using QtNodes::ConnectionStyle;

static void setNodeStyle()
{
	using namespace QtNodes;

	// Set style for node editor
	QFile nodeEditorStyleFile(":NodeEditorStyle.json");

	if (nodeEditorStyleFile.open(QIODevice::ReadOnly))
	{
		QString nodeEditorStyle = nodeEditorStyleFile.readAll();
		FlowViewStyle::setStyle(nodeEditorStyle);
		NodeStyle::setNodeStyle(nodeEditorStyle);
		ConnectionStyle::setConnectionStyle(nodeEditorStyle);
	}

	Connection::SetPolicy(Connection::Policy::OverrideExistingConnections);
}

int
main(int argc, char *argv[])
{
	std::cout
		<< "This is a DEVELOPMENT version of Gogh." << std::endl
		<< "Build number: 20201020-001" << std::endl
		<< "Copyright (c) 2018 - 2020 -- Gogh Team. All right reserved." << std::endl
		<< std::endl
		<< "This version is not meant for production and is still a work in progress." << std::endl
		<< "Please address all your remarks to the Gogh team on the bug tracker:" << std::endl
		<< "  https://github.com/eliemichel/CodenameGogh/issues" << std::endl
		<< std::endl
		<< "NB: ffmpeg and ffprobe commands must be installed and present in your PATH environment variable." << std::endl
		<< "    You can download it at: https://www.ffmpeg.org/download.html" << std::endl
		;
	QApplication app(argc, argv);

	setNodeStyle();
	app.setStyle(new MainWindowStyle());
	app.setPalette(app.style()->standardPalette());

	MainWindow w;

	// TEST
	//AddNodeMenu tmp;
	//tmp.showNormal();
	//return app.exec();
	// END TEST

	WelcomeDialog welcomeDialog;
	welcomeDialog.exec();

	switch (welcomeDialog.selectedAction()) {
	case WelcomeDialog::OpenGraph:
		w.openFile();
		break;
	case WelcomeDialog::NewGraph:
		break;
	default:
	case WelcomeDialog::Cancel:
		return EXIT_SUCCESS;
	}

	w.showNormal();
	w.raise();
  
	return app.exec();
}
