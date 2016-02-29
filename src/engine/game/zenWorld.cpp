#include "ZenWorld.h"
#include "zenconvert/zenParser.h"
#include "utils/logger.h"
#include <string>
#include "zenconvert/vob.h"
#include "zenconvert/zCMesh.h"
#include "vdfs/fileIndex.h"

#include "zenconvert/zCProgMeshProto.h"
#include "zenconvert/zenParser.h"
#include "engine.h"

#ifdef ZE_GAME
#include "renderer/visualLoad.h"
#include <RBuffer.h>
#include <RStateMachine.h>
#include <RDevice.h>
#include <RTools.h>
#include "renderer/renderSystem.h"
#include "renderer/visuals/visual.h"
#endif

using namespace Engine;

ZenWorld::ZenWorld(::Engine::Engine& engine, const std::string & zenFile, VDFS::FileIndex & vdfs, float scale)
{
	// Load zen from vdfs
	std::vector<uint8_t> data;
	vdfs.getFileData(zenFile, data);

	// Try to load from disk if this isn't in a vdf-archive
	if(data.empty())
	{
		ZenWorld::ZenWorld(engine, ZenConvert::ZenParser(zenFile), vdfs, scale);
	}
	else
	{
		// Load from memory
		ZenWorld::ZenWorld(engine, ZenConvert::ZenParser(data.data(), data.size()), vdfs, scale);
	}
}

ZenWorld::ZenWorld(::Engine::Engine& engine, ZenConvert::ZenParser& parser, VDFS::FileIndex & vdfs, float scale)
{
	// Load a world
	ZenConvert::zCMesh* worldMesh = nullptr;
	ZenConvert::oCWorldData worldData;

	// Try to parse the zen-file
	try
	{
		parser.readHeader();
		worldData = parser.readWorld();
		worldMesh = parser.getWorldMesh();
	}
	catch(std::exception &e)
	{
		LogError() << "Failed to load ZEN-File. Reason: " << e.what();
		return;
	}

	if(worldMesh)
		disectWorldMesh(worldMesh, engine, vdfs, scale);

	parseWorldObjects(worldData, engine, vdfs, scale);

#ifdef ZE_GAME
	engine.renderSystemPtr()->getPagedVertexBuffer<Renderer::WorldVertex>().RebuildPages();
	engine.renderSystemPtr()->getPagedIndexBuffer<uint32_t>().RebuildPages();
#endif
}

ZenWorld::~ZenWorld()
{

}

/**
* @brief Disects the worldmesh into its parts and creates the needed entities
*/
void ZenWorld::disectWorldMesh(ZenConvert::zCMesh* mesh, ::Engine::Engine& engine, VDFS::FileIndex & vdfs, float scale)
{
	std::vector<ObjectHandle> handles;

	ZenConvert::PackedMesh packedMesh;

	// Pack the mesh into an easier format
	mesh->packMesh(packedMesh, scale);

#ifdef ZE_GAME
	// Create the visual
	std::hash<std::string> hash;
	Renderer::Visual* pVisual = engine.renderSystemPtr()->createVisual(hash("__WORLDMESH"), packedMesh);	

	// Create entities for rendering	
	pVisual->createEntities(handles);
#endif

	// Create collisionmeshes for each material
	for(size_t s=0;s<handles.size();s++)
	{
		Components::Collision* pCc = engine.objectFactory().storage().addComponent<Components::Collision>(handles[s]);
		
		btTriangleMesh* wm = new btTriangleMesh;

		for(size_t i = 0; i < packedMesh.subMeshes[s].indices.size(); i+=3)
		{
			auto& v0 = packedMesh.vertices[packedMesh.subMeshes[s].indices[i]].Position;
			auto& v1 = packedMesh.vertices[packedMesh.subMeshes[s].indices[i+1]].Position;
			auto& v2 = packedMesh.vertices[packedMesh.subMeshes[s].indices[i+2]].Position;

			// Convert to btvector
			btVector3 v[] = {{v0.x, v0.y, v0.z}, {v1.x, v1.y, v1.z}, {v2.x, v2.y, v2.z}};
			wm->addTriangle(v[0], v[1], v[2]);
		}

		Physics::CollisionShape cShape(new btBvhTriangleMeshShape(wm, false));
		pCc->rigidBody.initPhysics(engine.physicsSystem(), cShape, Math::float3(0.0f, -1.0f, 0.0f));
		pCc->rigidBody.setRestitution(0.1f);
		pCc->rigidBody.setFriction(1.0f);
	}
}

/**
* @brief Creates entities for the loaded oCWorld
*/
void ZenWorld::parseWorldObjects(const ZenConvert::oCWorldData& data, ::Engine::Engine& engine, VDFS::FileIndex & vdfs, float scale)
{
	std::function<void(const std::vector<ZenConvert::zCVobData>&)> fn = [&](const std::vector<ZenConvert::zCVobData>& vobs)
	{
		for(auto& v : vobs)
		{
			// Same for child-vobs
			fn(v.childVobs);

#ifdef ZE_GAME
			// TODO: Put this into an other function
			if(v.visual.find(".3DS") != std::string::npos && v.visual.find(".3DS") != 0) // TODO: Don't find twice
			{
				Math::Matrix worldMatrix = v.rotationMatrix3x3.toMatrix((v.position - Math::float3(0,100,0)) * scale); // FIXME: Random as fuck, vobs are hovering 1m above the ground on scale of 1/100;
				float brightness = 1.0f;
				
				std::hash<std::string> hash;
				size_t visualHash = hash(v.visual);
				Renderer::Visual* pVisual = engine.renderSystemPtr()->getVisualByHash(visualHash);

				if(!pVisual)
				{
					// Strip .3DS-Part
					std::string vname = v.visual.substr(0, v.visual.find(".3DS"));
					// Try to find the mesh of this
					if(!vdfs.getFileByName(vname + ".MRM", nullptr)) // Try progmesh-proto
					{
						continue; // Skip this vob
					}

					// Found it, load the mesh-information
					ZenConvert::zCProgMeshProto mesh(vname + ".MRM", vdfs);

					// Create some entities using this visual
					ZenConvert::PackedMesh packedMesh;

					// Pack the mesh into an easier format
					mesh.packMesh(packedMesh, scale);

					// Create the visual
					pVisual = engine.renderSystemPtr()->createVisual(hash(v.visual), packedMesh);								
				}

				// Create entities and init visuals
				std::vector<ObjectHandle> handles;
				pVisual->createEntities(handles);

				Math::Matrix m = Math::Matrix::CreateIdentity();

				ZenConvert::VobObjectInfo ocb;
				ocb.worldMatrix = worldMatrix;
				ocb.color = Math::float4(brightness, brightness, brightness, 1.0f);
	
				// Create entities for each material
				RAPI::RBuffer* pObjectBuffer = nullptr;
				for(auto& h : handles)
				{
					if(!pObjectBuffer)
					{
						Components::Visual* pVisComp = engine.objectFactory().storage().getComponent<Components::Visual>(h);
						pObjectBuffer = pVisComp->pObjectBuffer;
						pObjectBuffer->UpdateData(&ocb);
						break;
					}

					// TODO: More here
				}
				
#endif
			}
		}
	};

	fn(data.rootVobs);
}

void ZenWorld::render(const Math::Matrix& viewProj)
{
#ifdef ZE_GAME
	for(auto& p : m_VobPositions)
	{
		RAPI::RTools::LineRenderer.AddPointLocator(RAPI::RFloat3(reinterpret_cast<float*>(&p)), 1.0f, RAPI::RFloat4(1,0,0,1));
	}
#endif
}