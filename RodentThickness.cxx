#include <iostream>
#include <string>

#include <QApplication>

#include "GuiCSV.h"

#include "RodentThicknessCLP.h"


int main(int argc, char* argv[])
{

	PARSE_ARGS;
	QApplication app(argc, argv);

	GuiCSV RodentThicknessGUI(dataset,configfile,PathBms,noGUI,WorkDir,argv[0]);

	if(noGUI) return RodentThicknessGUI.Compute();
	else
	{
		RodentThicknessGUI.show();
		return app.exec();
	}
	return -1;	

}
