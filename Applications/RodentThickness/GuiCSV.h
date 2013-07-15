#ifndef DEF_GuiCSV
#define DEF_GuiCSV
/*Qt classes*/
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QCloseEvent>
#include <QSignalMapper>
#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QStackedWidget>
#include <QLineEdit>
#include <QWidget>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QDebug>
#include "ui_guicsv.h"

#include "ScriptRunner.h"
#include "StatisticalpartRunner.h"
#include <signal.h> 
/*itk classes*/
#include "itkImage.h"
#include "itkImageFileReader.h"
#include <itksys/SystemTools.hxx> // for FindProgram() and GetFilenamePath()


class GuiCSV : public QMainWindow, public Ui::MainWindow 
{
	Q_OBJECT

	public:

/*CONSTRUCTOR*/	GuiCSV(std::string dataset,std::string configfile,std::string PathBms,bool noGui,bool ComputeStatistic, std::string WorkDir,int extractlabel,int dirichletLowId,int dirichletHighId, std::string commandRan); //constructor

/*DATASET*/	int ReadCSV(QString CSVfile); // returns -1 if fails, otherwise 0
		int SaveCSVDatasetBrowse(QString CSVBrowseName);
		int checkdataset();
		int checknumberofgroups();
/*CHECK IMAGE*/	int checkImage(std::string Image); 		
		void ConfigDefault();
/*SOFT CONFIG*/	int LoadConfig(QString ConfigFile); // returns -1 if fails, otherwise 0
		void SaveConfig(QString ConfigBrowseName);
/*MAIN FUNCT*/	int LaunchScriptWriter(); // returns -1 if failed, otherwise 0
		int LaunchScriptRunner();
		void RunningCompleted();
		void RunningFailed();
		
	public slots:

/*EXIT*/	void ExitProgram();
/*CASES*/	void OpenAddRow();
		void RemoveSelectedRow();
/*DATASET*/	void ReadCSVSlot();
		void SaveCSVDatasetBrowseSlot();
		void FullTableWidget(int,int);
/*SOFT CONFIG*/	void LoadConfigSlot();
		void SaveConfigSlot();
		void ConfigDefaultSlot();
		void BrowseSoft(int); //in the soft dialog window
		void ResetSoft(int);
/*WIDGETCHANGE*/void WidgetHasChangedParam();
/*READ ME*/	void ReadMe();
/*OUTPUT*/	void OpenOutputBrowseWindow();
/*MAIN FUNCT*/	int Compute();
		void testpythonversion();
		void ScriptQProcessDone(int);
		void ChangeValueParameters();
	signals: // none
	protected :

/*EXIT*/	void closeEvent(QCloseEvent* event);


	private:

		bool m_noGUI;
		bool m_ScriptRunning;
		bool m_computestatistic;
		bool m_ErrorDetectedInConstructor; // useful in --nogui mode to exit the program without trying the compute function	
		std::vector< std::string > m_FindProgramRTExecDirVec;
/*DATASET*/	QString m_CSVseparator;
/*PARAMETERS*/	int m_ParamSaved;		
		QString m_StatisticaldatasetPath;	
		QString m_OutputPath;
		QString m_FilesBmsPath;
		QString m_outconfig;
		QString m_outdataset;
		int m_label;
		int m_idl;
		int m_idh;
		int m_idn;
		std::string m_notFound;
		QProcess * m_ScriptQProcess;
		int m_statistic;
/*MAIN FUNCT*/	ScriptRunner* m_scriptrunner;
		StatisticalpartRunner*  m_statisticalpartrunner;
};
#endif
