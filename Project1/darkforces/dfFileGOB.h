#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <map>

struct GOB_Entry {
	long index;		// pointer to start of the file 
	long len;		// length of the file 
	char name[13];	// name of the file, // null terminated
};

struct GOB_Index {
	long nbFiles;	// number of files in the GOB INDEXES 
	GOB_Entry files[1]; // one index entry per file }
};

class dfFileGOB
{
	std::ifstream fd;
	GOB_Index* m_index;
	std::map<std::string, GOB_Entry> m_files;

public:
	dfFileGOB(std::string file);
	char* load(std::string file, int& size);
	~dfFileGOB();
};