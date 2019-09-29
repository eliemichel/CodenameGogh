#include <iostream>

#include <QApplication>

#include <nodes/ConnectionStyle>

#include "MainWindow.h"

using QtNodes::ConnectionStyle;

static
void
setStyle()
{
  ConnectionStyle::setConnectionStyle(
  R"(
  {
    "ConnectionStyle": {
      "ConstructionColor": "gray",
      "NormalColor": "black",
      "SelectedColor": "gray",
      "SelectedHaloColor": "deepskyblue",
      "HoveredColor": "deepskyblue",

      "LineWidth": 3.0,
      "ConstructionLineWidth": 2.0,
      "PointDiameter": 10.0,

      "UseDataDefinedColors": true
    }
  }
  )");
}

int
main(int argc, char *argv[])
{
	std::cout
		<< "This is a DEVELOPMENT version of Gogh." << std::endl
		<< "Build number: 20190930-001" << std::endl
		<< "Copyright (c) 2018 - 2019 -- Gogh Team. All right reserved." << std::endl
		<< std::endl
		<< "This version is not meant for production and is still a work in progress." << std::endl
		<< "Please address all your remarks to the Gogh team on the bug tracker:" << std::endl
		<< "  https://github.com/eliemichel/CodenameGogh/issues" << std::endl
		<< std::endl
		<< "NB: ffmpeg and ffprobe commands must be installed and present in your PATH environment variable." << std::endl
		<< "    You can download it at: https://www.ffmpeg.org/download.html" << std::endl
		;
	QApplication app(argc, argv);

	setStyle();

	MainWindow w;
	w.showNormal();
  
	return app.exec();
}
