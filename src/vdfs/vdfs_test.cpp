#include "archive_virtual.h"
#include "utils/logger.h"

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		LogError() << "Pass the VDF as argument!";
		return 0;
	}

	VDFS::ArchiveVirtual v;
	bool r = v.LoadVDF(argv[1]);

	if(r)
		LogInfo() << "Successfully read archive.";
	else
		LogError() << "Failed to read archive.";

	v.UpdateFileCatalog();

	return 0;
}
