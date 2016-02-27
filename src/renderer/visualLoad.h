#pragma once
#include "zenconvert/zCMesh.h"
#include <RFwdDecl.h>
#include <vector>
#include "engine/objecthandle.h"

namespace Engine
{
	class ObjectFactory;
}

namespace VDFS
{
	class FileIndex;
}

namespace Renderer
{
	/**
	 * @brief Loads a texture from the given VDF-Index
	 */
	RAPI::RTexture* loadTexture(const std::string& name, VDFS::FileIndex& fileIndex);

	/**
	 * @brief Creates entities for the given packed zCMesh. The handles are expected to be in the order matching the submeshes.
	 */
	void createVisualsFor(const ZenConvert::zCMesh::PackedMesh& packedMesh, Engine::ObjectFactory& factory, VDFS::FileIndex& vdfs, const std::vector<Engine::ObjectHandle>& handles);
}