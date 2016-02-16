#include "archive_virtual.h"
#include "utils/logger.h"

int main(void)
{
	VDFS::ArchiveVirtual v;
	bool r = v.LoadVDF("Meshes_Addon.vdf");

	if(r)
		LogInfo() << "Successfully read archive.";
	else
		LogError() << "Failed to read archive.";

	v.UpdateFileCatalog();

	return 0;
}
