// Qt includes
#include <QTimer>


#include "GuiCSV.h"

int main(int argc, char * argv[] )
{
  if( argc < 4 ) return -1;

  QApplication app(argc, argv); // create a QApplication

  GuiCSV TestGUI(argv[1],argv[2],"",false,false,"",3,4,1,argv[0]); // create a GuiCSV object : GuiCSV(CSVFile,ConfigFile,PathBms,noGUI,WorkDir,argv[0])

  TestGUI.show(); // open the window

  QTimer::singleShot(2000, qApp, SLOT(quit())); // close the window after 2000ms = 2s

  if(TestGUI.ImageMathPath->text().toStdString() != argv[3]) return -1;
  if(TestGUI.tableWidget->item(0,2)->text().toStdString() != argv[4]) return -1;

  return app.exec();
}
