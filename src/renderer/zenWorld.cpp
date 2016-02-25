#include "ZenWorld.h"
#include "zenconvert/zenParser.h"
#include "utils/logger.h"
#include <string>
#include "zenconvert/vob.h"
#include "zenconvert/zCMesh.h"
#include "renderer/zenWorldMesh.h"	
#include "vdfs/fileIndex.h"
#include <RTools.h>
#include "zenconvert/zCProgMeshProto.h"
#include "zenconvert/zenParser.h"


using namespace Renderer;

ZenWorld::ZenWorld(const std::string& zen)
{
	// Load a world
	const std::string zenFile = zen;
	ZenConvert::Chunk parentVob("parent", "", 0);
	ZenConvert::zCMesh* worldMesh = nullptr;

	ZenConvert::ZenParser parser(zenFile);

	try
	{
		
		parser.readHeader();
		parser.readWorld();
		worldMesh = parser.getWorldMesh();
	}
	catch(std::exception &e)
	{
		LogError() << e.what();
		return;
	}

	const float scale = 1.0f / 50.0f;

	VDFS::FileIndex vdfs;
	vdfs.loadVDF("Textures.vdf");
	vdfs.loadVDF("Textures_Addon.vdf");

	if(worldMesh)
		m_Meshes.emplace_back(new Renderer::ZenWorldMesh(*worldMesh, vdfs, scale));
	/*
	std::function<void(ZenConvert::Chunk*)> fn = [&](ZenConvert::Chunk* parent){
		for(uint32_t i = 0; i < parent->childCount(); i++)
		{
			auto v = parent->child(i);
			auto& ps = v->attributes()["trafoOSToWSPos"];

			Math::float3 position;
			sscanf(ps.c_str(), "vec3:%f %f %f", &position.x, &position.y, &position.z);

			m_VobPositions.push_back(position * scale);
			fn(parent->child(i));
		}
	};

	fn(&parentVob);*/


	/*VDFS::FileIndex vdfsIndex;
	vdfsIndex.loadVDF("Meshes.vdf");
	vdfsIndex.loadVDF("Meshes_Addon.vdf");

	float xoff = 0;
	for(auto& f : vdfsIndex.getKnownFiles())
	{
		if(f.fileName.find(".MRM") != std::string::npos)
		{
			ZenConvert::zCProgMeshProto proto(f.fileName, vdfsIndex);
			
			Math::float3 bbMin, bbMax;
			proto.getBoundingBox(bbMin, bbMax);
			xoff += bbMax.x - bbMin.x;

			m_Meshes.emplace_back(new Renderer::ZenWorldMesh(proto, scale, Math::float3(xoff * scale, 0, 0)));

			continue;
		}


		if(f.fileName.find(".MSH") == std::string::npos ||
			f.fileName.find("SKYDOME") != std::string::npos ||
			f.fileName.find("MAGICFRONTIER") != std::string::npos)
			continue;

		ZenConvert::zCMesh msh(f.fileName, vdfsIndex);

		Math::float3 bbMin, bbMax;
		msh.getBoundingBox(bbMin, bbMax);
		xoff += bbMax.x - bbMin.x;

		m_Meshes.emplace_back(new Renderer::ZenWorldMesh(msh, scale, Math::float3(xoff * scale, 0, 0)));

		

		LogInfo() << f.fileName << " Size: " << (bbMax - bbMin).length();
	}*/
}

Renderer::ZenWorld::ZenWorld(const std::string & zenFile, VDFS::FileIndex & vdfs)
{
	// Load zen from vdfs
	std::vector<uint8_t> data;
	vdfs.getFileData(zenFile, data);

	// Load a world
	ZenConvert::zCMesh* worldMesh = nullptr;
	ZenConvert::ZenParser parser(data.data(), data.size());

	try
	{
		parser.readHeader();
		parser.readWorld();
		worldMesh = parser.getWorldMesh();
	}
	catch(std::exception &e)
	{
		LogError() << e.what();
		return;
	}

	const float scale = 1.0f / 50.0f;

	if(worldMesh)
		m_Meshes.emplace_back(new Renderer::ZenWorldMesh(*worldMesh, vdfs, scale));
}

ZenWorld::~ZenWorld()
{

}

void ZenWorld::render(const Math::Matrix& viewProj)
{
	RAPI::RRenderQueueID queueID = RAPI::REngine::RenderingDevice->AcquireRenderQueue(true);

	for(auto& m : m_Meshes)
	{
		m->render(viewProj, queueID);
	}

	RAPI::REngine::RenderingDevice->ProcessRenderQueue(queueID);

	for(auto& p : m_VobPositions)
	{
		RAPI::RTools::LineRenderer.AddPointLocator(RAPI::RFloat3(reinterpret_cast<float*>(&p)), 1.0f, RAPI::RFloat4(1,0,0,1));
	}
}