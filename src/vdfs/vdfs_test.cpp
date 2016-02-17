#include "fileIndex.h"
#include "utils/logger.h"

int main(int argc, char** argv)
{
	/*if(argc != 2)
	{
		LogError() << "Pass the VDF as argument!";
		return 0;
	}*/

	VDFS::FileIndex idx;

	idx.loadVDF("Textures.vdf");
	idx.loadVDF("Meshes_Addon.vdf");
	idx.loadVDF("testmod.mod");

	std::vector<uint8_t> testData;
	idx.getFileData("test.hlsl", testData);

	return 0;
}
