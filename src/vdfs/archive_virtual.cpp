#include "archive_virtual.h"
#include "utils/logger.h"
#include "fileIndex.h"
#include <stack>
#include <functional>
#include <string.h>

/**
 * Quick format rundown:
 *
 * Each file starts with a header, described by the type "VdfHeader".
 * For this to be a valid file, each header has to have the version-property set to 0x50.
 * The signature can either be the string in VDF_SIGNATURE_G1 or VDF_SIGNATURE_G2, depending
 * on the game-version this was built with.
 *
 * After the header, the root file catalog can be found, consisting out of Header.NumEntries entries of the type VdfEntryInfo.
 */

using namespace VDFS;

const char* VDF_SIGNATURE_G1 = "PSVDSC_V2.00\r\n\r\n";
const char* VDF_SIGNATURE_G2 = "PSVDSC_V2.00\n\r\n\r";

ArchiveVirtual::ArchiveVirtual() : 
	m_pStream(nullptr)
{
}

ArchiveVirtual::~ArchiveVirtual()
{
	if(m_pStream)
		fclose(m_pStream);
}

/**
* @brief Loads the given VDFS-File and initializes the index
*/
bool ArchiveVirtual::LoadVDF(const std::string& file)
{
	if(m_pStream)
	{
		LogWarn() << "Cannot re-use a virtual archive!";
		return false;
	}

	// Open the archive
	m_pStream = fopen(file.c_str(), "rb");

	if(!m_pStream)
		return false;

	// Read and verify header
	fread(&m_VdfHeader, sizeof(m_VdfHeader), 1, m_pStream);

	// Verify header version
	if(m_VdfHeader.Version != 0x50)
	{
		LogError() << "VDFS-File '" << file << "' has an invalid header-version of: 0x" << std::hex << m_VdfHeader.Version;
		fclose(m_pStream); m_pStream = nullptr;
		return false;
	}

	// Check for game signature
	if(memcmp(m_VdfHeader.Signature, VDF_SIGNATURE_G1, strlen(VDF_SIGNATURE_G1)) == 0)
		m_ArchiveVersion = AV_Gothic1;
	else if(memcmp(m_VdfHeader.Signature, VDF_SIGNATURE_G2, strlen(VDF_SIGNATURE_G2)) == 0)
		m_ArchiveVersion = AV_Gothic2;
	else
	{
		LogError() << "Unknown VDF-Archive signature on file '" << file << "'";
		fclose(m_pStream); m_pStream = nullptr;
		return false;
	}

	return true;
}

/**
* @brief Updates the file catalog of this archive
*/
bool ArchiveVirtual::UpdateFileCatalog()
{
	if(!m_pStream)
	{
		LogError() << "VDF-File not initialized!";
		return false;
	}

	// Skip to file catalog
	fseek(m_pStream, sizeof(VdfHeader), SEEK_SET);

	// Allocate memory for the catalog
	m_EntryCatalog.resize(m_VdfHeader.NumEntries);

	// Read catalog
	if(m_VdfHeader.NumEntries != fread(m_EntryCatalog.data(), sizeof(VdfEntryInfo), m_VdfHeader.NumEntries, m_pStream))
	{
		LogError() << "Failed to read VDFS-Root catalog";
		m_EntryCatalog.clear();
		return false;
	}

	// Fix filenames. They're missing a terminating 0.
	for(size_t total = 0; total < m_EntryCatalog.size(); total++)
	{
		auto& e = m_EntryCatalog[total];

		// Fill the name-buffer with 0 until we reach a non-empty character, as
		// these are not 0 terminated
		// FIXME: This could give trouble with names which are actually 64 chars long!
        e.Name[63] = '\0';
        for(size_t i = 63; i >= 0; i--)
		{
			if(e.Name[i] != ' ')
				break;

			e.Name[i] = 0;
		}
	}

	// List all folders and files to console
	std::function<void(int,int,bool)> f = [&](int idx, int level, bool dirsOnly) {
		auto& e = m_EntryCatalog[idx];

		// List files in directory
		do
		{
			std::string tabs = std::string(level, '|');

			if(!dirsOnly || m_EntryCatalog[idx].Type & VDF_ENTRY_DIR)
				LogInfo() << "Entry: " << tabs << m_EntryCatalog[idx].Name;

			if(m_EntryCatalog[idx].Type & VDF_ENTRY_DIR)
			{
				f(m_EntryCatalog[idx].JumpTo, level + 1, dirsOnly);
				LogInfo() << "Entry: " << tabs << "END " << m_EntryCatalog[idx].Name;
			}
			idx++;
		}while(!(m_EntryCatalog[idx-1].Type & VDF_ENTRY_LAST));
	};

	// List everything
	f(0, 0, false);

	// Print only directory structure
	f(0, 0, true);
}
