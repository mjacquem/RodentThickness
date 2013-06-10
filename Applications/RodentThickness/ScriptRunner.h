#ifndef DEF_SCRIPTRUNNER
#define DEF_SCRIPTRUNNER

/*std classes*/
#include <iostream>
#include <string>
#include <vector>





class ScriptRunner
{
public:
	void readFileCSV(std::string line, std::string &variable1, std::string &variable2,std::string &variable3);
	int GetNumberGroups(std::string file);
	int GetGroups(std::string file, std :: vector<std :: string>  &groupIds , std :: vector<std :: string>  &subjgroups, std :: vector<std :: string>  &Groups, int ordercsv);
	int RunBatchmakeScript(std::string dataset,std::string configfile,std::string PathBms,std::string WorkDir); 

};
#endif
