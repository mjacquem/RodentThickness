#include <iostream>
#include <string>

#include <QApplication>

#include "GuiCSV.h"

#include "RodentThicknessCLP.h"


int main(int argc, char* argv[])
{

	PARSE_ARGS;
  //thanks to this line, we can use the variables entered in command line as variables of the program
  //std::string dataset,std::string configfile,std::string PathBms,bool noGui,bool ComputeStatistic, std::string WorkDir,int extractlabel,int dirichletLowId,int dirichletHighId, 
	QApplication app(argc, argv);

	GuiCSV RodentThicknessGUI(dataset,configfile,PathBms,noGUI,ComputeStatistic,WorkDir,extractlabel,dirichletLowId,dirichletHighId,argv[0]);
	/* Launch App */
	if(noGUI) return RodentThicknessGUI.Compute();
	else
	{
		RodentThicknessGUI.show();
		return app.exec();
	}
	return -1;	

}
