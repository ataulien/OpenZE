#include "fileIndex.h"
#include "utils/logger.h"
#include "zenconvert/ztex2dds.h"

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
	idx.getFileData("NW_DUNGEON_WALL_01-C.TEX", testData);

	std::vector<uint8_t> ddsData;
	ZenConvert::convertZTEX2DDS(testData, ddsData);

	FILE* f = fopen("NW_DUNGEON_WALL_01.dds", "wb");
	fwrite(ddsData.data(), ddsData.size(), 1, f);

	fclose(f);

	return 0;
}
