# include <iostream>

#ifdef _unix || _linux
#include <unistd.h>
#endif


#include "string"
#include "errno.h"
#include <fstream>
#include <vector>
#include "bmScriptParser.h"
#include <stdio.h>
#include <stdlib.h>

#include "StatisticalpartRunner.h"

void StatisticalpartRunner :: readFileCSV(std::string line, std::string &variable1, std::string &variable2,std::string &variable3,std::string &variable4,std::string &variable5,std::string &variable6,std::string &variable7,std::string &variable8)
{
	int pos1=0, pos2=0, pos3=0, pos4=0, pos5=0, pos6=0, pos7=0, pos8=0;

	pos1 = line.find(',',0);
	variable1.assign(line,0,pos1);			
				
 	pos2 = line.find(',',pos1+1);
	variable2.assign(line,pos1+1,pos2-pos1-1);		
			
	pos3 = line.find(',',pos2+1);
	variable3.assign(line,pos2+1,pos3-pos2-1);		
	
	pos4 = line.find(',',pos3+1);
	variable4.assign(line,pos3+1,pos4-pos3-1);	

	pos5 = line.find(',',pos4+1);
	variable5.assign(line,pos4+1,pos5-pos4-1);	

	pos6 = line.find(',',pos5+1);
	variable6.assign(line,pos5+1,pos6-pos5-1);	

	pos7 = line.find(',',pos6+1);
	variable7.assign(line,pos6+1,pos7-pos6-1);	

	pos8 = line.find(',',pos7+1);
	variable8.assign(line,pos7+1,pos8-pos7-1);				   
}
int StatisticalpartRunner :: GetNumberGroups(std::string file)
{
    std::ifstream groups(file.c_str(), std::ios::in);
    int c = 0;
    
    if (groups)
    {
        std::string s;
	while ( getline(groups,s) ) c++;
	groups.close();
    }
    else std::cerr << "File doesn't exist" << std::endl;
    
    return c;
}
int StatisticalpartRunner :: GetGroups(std::string file, std :: vector<std :: string>  &groupIds , std :: vector<std :: string>  &subjgroups)
{
    std::ifstream groups(file.c_str(), std::ios::in);
    int first_line = 0;
    if (groups)
    {
        std::string s;
        while ( getline(groups,s) )
	{
		if (first_line != 0)
		{	
			int test = 0;
			std :: string variable1 , variable2, variable3, variable4, variable5, variable6, variable7, variable8;
			std :: string groupname, subjname;
			readFileCSV(s, variable1,variable2,variable3,variable4,variable5,variable6,variable7,variable8);

				groupname=variable1;
				subjname=variable2;
			
			std :: vector<std :: string> :: iterator i=groupIds.begin();
			string::size_type s;
			while(i != groupIds.end())
			{ 
				s=groupname.find(*i,0);
				if( s==0) test=1;
				i++;
			}
			//New entry if test = 0
			if (test == 0)
			{	
				groupIds.push_back(groupname);
				
			}
			subjgroups.push_back(subjname);
			
		}
		first_line = 1;
		s.clear();
        }
        groups.close();
    }
    else 
    {  std::cerr << "File doesn't exist" << std::endl;
       return -1;  }
  return groupIds.size();
}

int StatisticalpartRunner :: RunBatchmakeStatisticalScript(std::string dataset,std::string configfile,std::string PathBms,std::string WorkDir)
{
// Run 3 batchmake scripts 	(Data)
////////////////////////////////////////////////////////////////////////////////////////////////////////Pipeline 1//////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
	std::string nameFile2;
	nameFile2=dataset;	
	int number_of_subject = GetNumberGroups(nameFile2);
	number_of_subject = number_of_subject - 1; // First line isn't a group name
 	int error=0;
//Initialisations
	std :: vector<std :: string> groupIds, subj;
	std::string final_groups, subjGroup;

	int L = GetGroups(nameFile2, groupIds, subj);  
	if(L==-1) return -1;
	int Length=subj.size();

	for (int i=0;i<L;++i)
  	{  		 
		if (i != L-1) final_groups = final_groups + groupIds[i] + ' ';
    		else final_groups = final_groups + groupIds[i];
 	 }
 	for(int i = 0; i < Length; ++i)
  	{ 
   		 if (i != Length-1) subjGroup = subjGroup + subj[i] + ' ';
    		else subjGroup = subjGroup + subj[i];
 	 }
	std::string BatchMakeScriptFile2 = WorkDir+"/Script/"+"slicer3createfiletxt.bms"; //create a new batchmake script
	std::ofstream file2( BatchMakeScriptFile2.c_str());
 
	file2 <<"set (groupIds "<< final_groups<<")"<<std::endl;
	file2 <<"set (subjects "<< subjGroup<<")"<<std::endl;
	file2 <<"set (WorkDir "<< WorkDir<<")"<<std::endl;
	file2 <<"include ("<<configfile<<")"<<std::endl;
	file2 <<"include ("<<PathBms<<"/createfiletxt.bms)"<<std::endl; 
	file2.close();  

	bm::ScriptParser m_Parser1;
	if(m_Parser1.Execute(BatchMakeScriptFile2) == false ) error=1; // Run the new batchmake script where createfiletxt.bms is included

//////////////////////////////////////////////////////////////////////////////////////////////////////////////Pipeline2/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
string surfacemodel;
if(error==0)
{

	std::string nameFile;
	nameFile=dataset;	
	std::ifstream Sub(nameFile.c_str(),std::ios::in);
	int i=0;
	
	if(Sub)
	{
		std :: string line;
		std :: string column1,column2,column3,column4,column5,column6,column7,column8;
		while(getline( Sub, line ))
		{  			
					std :: string value1,value2,value3,value4,value5,value6,value7,value8;
				if(i==0)
				{
					readFileCSV(line, column1,column2,column3,column4,column5,column6,column7,column8);
					if(column1.compare("group") != 0 && column2.compare("subjId") != 0  && column5.compare("surfacemodels") != 0 ) 
					{	std :: cout << " Name of the Column 1 or 2 or 5 or 8 is not good : group , sudjId, labelMapInput" << std :: endl ;
						return -1;	
					}
				}
				if (i>0)
				{
					std::string BatchMakeScriptFile = WorkDir+"/Script/"+"slicer3filetxt.bms"; //create a new batchmake script
					std::ofstream file( BatchMakeScriptFile.c_str());
					readFileCSV(line, value1,value2,value3,value4,value5,value6,value7,value8);
					file <<"set (case1 "<< value2<<")"<<std::endl;
					file <<"set (group "<< value1<<")"<<std::endl;
					file <<"set (surfacemodels "<< value5<<")"<<std::endl;
					file <<"set (attributefilename "<< value7<<")"<<std::endl;	
					file <<"set (WorkDir "<< WorkDir<<")"<<std::endl;
					file <<"include ("<<configfile<<")"<<std::endl;
					file <<"include ("<<PathBms<<"/file.bms)"<<std::endl;
					if(i==1) surfacemodel=value5;
					file.close();
					
					bm::ScriptParser m_Parser; 
					if(m_Parser.Execute(BatchMakeScriptFile) ==false) error=1; 	 // Run the new batchmake script where file.bms is included
				}
			i++;
		}
		Sub.close();
	} // fin du if (sub)
	else
	{
	      std::cout << "ERROR: Unable  to open file for reading." << std::endl;
	return -1;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////Pipeline3////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//file bms
if(error==0)
{
	std::string BatchMakeScriptFile1 = WorkDir+"/Script/"+"slicer3correspondence.bms"; //create a new batchmake script
	std::ofstream file6( BatchMakeScriptFile1.c_str());
	file6 <<"set (surfacemodels "<< surfacemodel<<")"<<std::endl;
	file6 <<"set (groupIds "<< final_groups<<")"<<std::endl;
	file6 <<"set (WorkDir "<< WorkDir<<")"<<std::endl;
	file6 <<"set (subjects "<< subjGroup<<")"<<std::endl;
	file6 <<"include ("<<configfile<<")"<<std::endl;
	file6 <<"include ("<<PathBms<<"/correspondance.bms)"<<std::endl;   
	file6.close();  

	bm::ScriptParser m_Parser2;
	m_Parser2.Execute(BatchMakeScriptFile1);	// Run the new batchmake script where correspondance.bms is included
}
if(error == 1) return -1;
return 0;
	
}





