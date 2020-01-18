#include "dfFileGOB.h"

struct GOB_Header {
	char magic[4];	 // 'GOB' followed by 0x0A 
	long index;		// offset to MASTERN }
};

dfFileGOB::dfFileGOB(std::string file)
{
	fd = std::ifstream(file, std::ios::in | std::ios::binary);

	// check magic an find master index
	GOB_Header header;
	fd.read((char*)&header, sizeof(GOB_Header));
	if (strncmp(header.magic, "GOB\n", 4) != 0) {
		std::cerr << "dfFileGOB::dfFileGOB << file << is not a GOB file" << std::endl;
		exit(-1);
	}

	// read first entry of the master index
	long nbFiles;
	fd.seekg(header.index);
	fd.read((char*)&nbFiles, sizeof(long));

	// and re-index
	GOB_Entry entry;
	long index, size;
	char name[13];

	for (long i = 0; i < nbFiles; i++) {
		fd.read((char*)&index, sizeof(long));
		fd.read((char*)&size, sizeof(long));
		fd.read((char*)name, 13);
		entry.index = index;
		entry.len = size;
		strncpy_s(entry.name, name, 13);
		m_files[name] = entry;
	}
}

/**
 * Load a file from the GOb and return as array of char
 */
char* dfFileGOB::load(std::string file)
{
	if (m_files.count(file) == 0) {
		return nullptr;
	}

	GOB_Entry& entry = m_files[file];

	fd.seekg(entry.index);
	char* data = (char *)calloc(1, entry.len);
	fd.read(data, entry.len);

	return data;
}

dfFileGOB::~dfFileGOB()
{
	free(m_index);
	fd.close();
}
