#ifndef DEF_STATISTICALPARTRUNNER
#define DEF_STATISTICALPARTRUNNER

/*std classes*/
#include <iostream>
#include <string>
#include <vector>



class StatisticalpartRunner
{
public:
	void readFileCSV(std::string line, std::string &variable1, std::string &variable2,std::string &variable3,std::string &variable4,std::string &variable5,std::string &variable6,std::string &variable7,std::string &variable8);
	int GetNumberGroups(std::string file);
	int GetGroups(std::string file, std :: vector<std :: string>  &groupIds , std :: vector<std :: string>  &subjgroups);
	int RunBatchmakeStatisticalScript(std::string dataset,std::string configfile,std::string PathBms,std::string WorkDir); 
};
#endif
