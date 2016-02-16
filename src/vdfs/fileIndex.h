#pragma once
#include <string>
#include <unordered_map>
#include <vector>

// TODO: Repace with general class and make a forward decl!
#include "archive_virtual.h"

namespace VDFS
{
	/**
	 * @brief Information about in which archive the file is and on what offset it starts
	 */
	struct FileInfo
	{
		std::string fileName;
		uint32_t fileSize;
		Archive* targetArchive; 
		uint32_t archiveOffset; // 0 for physical files
	};

	class FileIndex
	{
	public:
		FileIndex();
		~FileIndex();

		/**
		 * @brief Places a file into the index
		 * @return True if the file was new, false otherwise
		 */
		bool AddFile(const FileInfo& inf);

		/**
		 * @brief Replaces a file matching the same name
		 * @return True, if the file was actually replaced. False if it was just added because it didn't exist
		 */
		bool ReplaceFileByName(const FileInfo& inf);

		/**
		 * @brief Fills the given pointer with the information about the provided filename.
		 * @return False, if the file was not found
		 */
		bool GetFileByName(const std::string& name, FileInfo* outinf);

		/**
		 * @brief Clears the complete index and all registered files
		 */
		void ClearIndex();

	private:
		/**
		 * @brief Vector of all known files
		 */
		std::vector<FileInfo> m_KnownFiles;

		/**
		 * @brief Map of indices of m_KnownFiles by their filenames 
		 */
		std::unordered_map<std::string, size_t> m_FileIndicesByName;
	};
}