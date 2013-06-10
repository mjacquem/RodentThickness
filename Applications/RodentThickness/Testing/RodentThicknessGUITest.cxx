// Qt includes
#include <QTimer>


#include "GuiCSV.h"

int main(int argc, char * argv[] )
{
  QApplication app(argc, argv); // create a QApplication

  GuiCSV TestGUI("",argv[1],"",false,false,"",argv[0]); // create a GuiCSV object : GuiCSV(CSVFile,ConfigFile,PathBms,noGUI,WorkDir,argv[0])

  TestGUI.show(); // open the window

  QTimer::singleShot(2000, qApp, SLOT(quit())); // close the window after 2000ms = 2s

  return app.exec();
}
