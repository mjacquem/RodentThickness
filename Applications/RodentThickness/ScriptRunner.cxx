/*std classes*/
#include <iostream>
#include <string>
#include <vector>
// #include <stdio.h>
//#include <unistd.h>
#include "errno.h"
#include <fstream>
#include "bmScriptParser.h"
#include <stdio.h>
#include <stdlib.h>

#include "ScriptRunner.h"

void ScriptRunner :: readFileCSV(std::string line, std::string &variable1, std::string &variable2,std::string &variable3)
{
	int pos1=0, pos2=0, pos3=0;

	pos1 = line.find(',',0);
	variable1.assign(line,0,pos1);			
				
 	pos2 = line.find(',',pos1+1);
	variable2.assign(line,pos1+1,pos2-pos1-1);		
			
	pos3 = line.find(',',pos2+1);
	variable3.assign(line,pos2+1,pos3-pos2-1);	

}
int ScriptRunner :: GetNumberGroups(std::string file)
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
int ScriptRunner :: GetGroups(std::string file, std :: vector<std :: string>  &groupIds , std :: vector<std :: string>  &subjgroups, std :: vector<std :: string>  &Groups, int ordercsv)
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
			std :: string variable1 , variable2, variable3;
			std :: string groupname, subjname;
			readFileCSV(s, variable1,variable2,variable3);

			
				groupname=variable3;
				subjname=variable1;
			

			
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
			Groups.push_back(groupname);
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

int ScriptRunner :: RunBatchmakeScript(std::string dataset,std::string configfile,std::string PathBms,std::string WorkDir, int extractlabel, int idl , int idh)
{
////////////////////////////////////////////////////////////////////////////////////////////////////////Pipeline 1//////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
	std::string nameFile;
	nameFile=dataset;	
	std::ifstream Sub(nameFile.c_str(),std::ios::in);
	int i=0, ordercsv=0;
	
	if(Sub)
	{
		std :: string line;
		std :: string column1,column2,column3;
		while(getline( Sub, line ))
		{  			
					std :: string value1,value2,value3;
					std::string BatchMakeScriptFile = WorkDir+"/Script/"+"slicer3ThicknessSPHARM.bms";
					std::ofstream file( BatchMakeScriptFile.c_str());
				
				if(i==0)
				{		
					
					readFileCSV(line, column1,column2,column3);
	
					/*if(column1.compare("subjId") != 0 && column2.compare("labelMapInput") != 0 && column3.compare("group") != 0 ) 
					{	std :: cout << " Name of the Columns is not good : group , sudjId, labelMapInput" << std :: endl ;
						return -1;	}*/
					
				}
				if (i>0)
				{	
					readFileCSV(line, value1,value2,value3);
						
					file <<"set (group "<< value3<<")"<<std::endl;
					file <<"set (case1 "<< value1<<")"<<std::endl;
					file <<"set (labelMapInput "<< value2<<")"<<std::endl;
					file <<"set (WorkDir "<< WorkDir<<")"<<std::endl;
					file <<"set (configfile "<< configfile<<")"<<std::endl;
					file <<"set (extractlabel "<< extractlabel<<")"<<std::endl;
					file <<"set (idl "<< idl<<")"<<std::endl;
					file <<"set (idh "<< idh<<")"<<std::endl;
					file <<"include ("<<configfile<<")"<<std::endl;
					file <<"include ("<<PathBms<<"/ThicknessSpharm.bms)"<<std::endl;
					file.close();
					
					bm::ScriptParser m_Parser;
					m_Parser.Execute(BatchMakeScriptFile); 	
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
//////////////////////////////////////////////////////////////////////////////////////////////////////Pipeline2////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	std::string nameFile2;
	nameFile2=dataset;	
	int number_of_subject = GetNumberGroups(nameFile2);
	number_of_subject = number_of_subject - 1; // First line isn't a group name
 	
//Initialisations
	std :: vector<std :: string> groupIds, Groups, subj;
	std::string final_groups, subjGroup;

	int L = GetGroups(nameFile2, groupIds, subj, Groups,ordercsv);  
	if(L==-1) return -1;
	int Length=Groups.size();

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
  	
//file bms
	std::string BatchMakeScriptFile2 = WorkDir+"/Script/"+"slicer3shapeworks.bms";
	std::ofstream file2( BatchMakeScriptFile2.c_str());
 
	file2 <<"set (groupIds "<< final_groups<<")"<<std::endl;
	file2 <<"set (subjects "<< subjGroup<<")"<<std::endl;
	file2 <<"set (WorkDir "<< WorkDir<<")"<<std::endl;
	file2 <<"set (configfile "<< configfile<<")"<<std::endl;	
	file2 <<"include ("<<configfile<<")"<<std::endl;
	file2 <<"include ("<<PathBms<<"/shapeworks.bms)"<<std::endl;
	file2.close();  

	bm::ScriptParser m_Parser1;
	m_Parser1.Execute(BatchMakeScriptFile2);
////////////////////////////////////////////////////////////////////////////////////////////////Pipeline3/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	std::ifstream Sub2(nameFile.c_str(),std::ios::in);
	int i2=0;
	if(Sub2)
	{
		std :: string line;
		while(getline( Sub2, line ))
		{  
			if (i2>0)
			{
							
				std :: string value1,value2,value3;
			
				std::string BatchMakeScriptFile5 = WorkDir+"/Script/"+"slicer3meshintensity.bms";
				std::ofstream file( BatchMakeScriptFile5.c_str());

				readFileCSV(line, value1,value2,value3);
				
				file <<"set (labelMapInput "<< value2<<")"<<std::endl;
				file <<"set (case1 "<< value1<<")"<<std::endl;
				file <<"set (group "<< value3<<")"<<std::endl;
				file <<"set (WorkDir "<< WorkDir<<")"<<std::endl;
				file <<"set (configfile "<< configfile<<")"<<std::endl;
				file <<"set (ids "<< extractlabel<<")"<<std::endl;
				file <<"set (idl "<< idl<<")"<<std::endl;
				file <<"set (idh "<< idh<<")"<<std::endl;
				//file <<"include (/work/mjacquem/Slicer3RodentThicknessProcessing/SampleBatchMake/meshintensity.bms)"<<std::endl;
				file <<"include ("<<configfile<<")"<<std::endl;
				file <<"include ("<<PathBms<<"/meshintensity.bms)"<<std::endl;
				file.close();
				
				bm::ScriptParser m_Parser;
				m_Parser.Execute(BatchMakeScriptFile5); 	
			}
			i2++;
		}
		Sub2.close();
	} // fin du if (sub)
	else
	{
	      std::cout << "ERROR: Unable  to open file for reading." << std::endl;
		return -1;
	}
return 0;

}
























