#include "fileIndex.h"
#include "utils/logger.h"

using namespace VDFS;

FileIndex::FileIndex()
{
}

FileIndex::~FileIndex()
{

}

/**
* @brief Places a file into the index
* @return True if the file was new, false otherwise
*/
bool FileIndex::AddFile(const FileInfo& inf)
{
	// Already exists?
	if(m_FileIndicesByName.find(inf.fileName) != m_FileIndicesByName.end())
		return false;

	// Add to known files and register in the index-map
	m_KnownFiles.push_back(inf);
	m_FileIndicesByName[inf.fileName] = m_KnownFiles.size() - 1;

	return true;
}

/**
* @brief Replaces a file matching the same name
* @return True, if the file was actually replaced. False if it was just added because it didn't exist
*/
bool FileIndex::ReplaceFileByName(const FileInfo& inf)
{
	// Check if the file even exists first
	auto it = m_FileIndicesByName.find(inf.fileName);
	if(it == m_FileIndicesByName.end())
	{
		// It doesn't, just add it
		AddFile(inf);
		return false;
	}

	// It does exist, replace it
	m_KnownFiles[(*it).second] = inf;
	return true;
}

/**
* @brief Fills the given pointer with the information about the provided filename.
* @return False, if the file was not found
*/
bool FileIndex::GetFileByName(const std::string& name, FileInfo* outinf)
{
	// Does the file even exist?
	auto it = m_FileIndicesByName.find(name);
	if(it == m_FileIndicesByName.end())
		return false;

	// Output the file information
	*outinf = m_KnownFiles[(*it).second];

	return true;
}

/**
* @brief Clears the complete index and all registered files
*/
void FileIndex::ClearIndex()
{
	m_FileIndicesByName.clear();
	m_KnownFiles.clear();
}
