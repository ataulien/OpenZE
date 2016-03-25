#include "visualLoad.h"
#include "engine/objectfactory.h"
#include "vdfs/fileIndex.h"
#include <REngine.h>
#include <RResourceCache.h>
#include "zenconvert/ztex2dds.h"
#include <RTexture.h>
#include <RTools.h>
#include <RStateMachine.h>
#include <RDevice.h>
#include <RBuffer.h>
#include "engine/components/visual.h"
#include "zenconvert/zTypes.h"
#include "renderer/vertextypes.h"
#include "zenconvert/ztex.h"

const std::string DEFAULT_TEXTURE = "DEFAULT_TEXTURE.TEX";

#ifdef __ANDROID__
#define UNPACK_DDS
#endif

/**
* @brief Loads a texture from the given VDF-Index
*/
RAPI::RTexture* Renderer::loadTexture(const std::string& _name, const VDFS::FileIndex& fileIndex)
{
	std::string name = _name;
	if(name.empty())		
		name = DEFAULT_TEXTURE; // Load default texture

	RAPI::RTexture* tx = RAPI::REngine::ResourceCache->GetCachedObject<RAPI::RTexture>(name);
	if(tx)
		return tx;

	// Get from .TGA to -C.TEX
	std::string fileName = name.substr(0, name.find_first_of('.'));
	fileName += "-C.TEX";

	// Read data from vdfs
	std::vector<uint8_t> textureData;
	if(!fileIndex.getFileData(fileName, textureData))
	{
		tx = RAPI::REngine::ResourceCache->GetCachedObject<RAPI::RTexture>(name);

		if(!tx)
		{
			if(!fileIndex.getFileData(DEFAULT_TEXTURE, textureData))
				return nullptr;
		}
		else
		{
			return tx;
		}
	}

	// Convert to actual dds
	std::vector<uint8_t> ddsData;
	std::vector<uint8_t> rgba8data;
	ZenConvert::convertZTEX2DDS(textureData, ddsData);

	tx = RAPI::REngine::ResourceCache->CreateResource<RAPI::RTexture>();

#ifdef UNPACK_DDS
	ZenConvert::convertDDSToRGBA8(ddsData, rgba8data, 2);

	ZenConvert::DDSURFACEDESC2* desc = reinterpret_cast<ZenConvert::DDSURFACEDESC2*>(ddsData.data() + 4);
	
	// Upload texture
	
	tx->CreateTexture(rgba8data.data(), rgba8data.size(), RAPI::RInt2(desc->dwWidth/4,desc->dwHeight/4), 1, RAPI::TF_R8G8B8A8);
#else
	tx->CreateTexture(ddsData.data(), ddsData.size(), RAPI::RInt2(0,0), 0, RAPI::TF_FORMAT_UNKNOWN_DXT);
#endif

	// Add to cache as the inputname
	RAPI::REngine::ResourceCache->AddToCache(name, tx);

	return tx;
}

/**
* @brief Creates entities for the given packed zCMesh
*/
void Renderer::createVisualsFor(RenderSystem& system, const ZenConvert::PackedMesh& packedMesh, Engine::ObjectFactory& factory, VDFS::FileIndex& vdfs, const std::vector<Engine::ObjectHandle>& handles)
{
	// Create buffers and states for each texture
	RAPI::RPixelShader* ps = RAPI::REngine::ResourceCache->GetCachedObject<RAPI::RPixelShader>("simplePS");
	RAPI::RVertexShader* vs = RAPI::REngine::ResourceCache->GetCachedObject<RAPI::RVertexShader>("simpleVS");
	RAPI::RStateMachine& sm = RAPI::REngine::RenderingDevice->GetStateMachine();

	RAPI::RInputLayout* inputLayout = RAPI::RTools::CreateInputLayoutFor<Renderer::WorldVertexInstanced>(vs);
	RAPI::RSamplerState* ss;
	RAPI::RRasterizerState* rs;
	RAPI::RTools::MakeDefaultStates(nullptr, &ss, nullptr, &rs);

	RAPI::RSamplerStateInfo ssi;
	ssi.SetDefault();
	ssi.MaxAnisotropy = 16.0f;
	ssi.Filter = RAPI::FILTER_ANISOTROPIC;
	ss = RAPI::RTools::GetState(ssi);

	sm.SetSamplerState(ss);
	sm.SetRasterizerState(rs);
	sm.SetPixelShader(ps);
	sm.SetVertexShader(vs);
	sm.SetInputLayout(inputLayout);

	RAPI::RBuffer* pObjectBuffer = RAPI::REngine::ResourceCache->CreateResource<RAPI::RBuffer>();
	sm.SetConstantBuffer(1, pObjectBuffer, RAPI::ST_VERTEX);
	sm.SetConstantBuffer(0, RAPI::REngine::ResourceCache->GetCachedObject<RAPI::RBuffer>("PerFrameCB"), RAPI::ST_VERTEX);

	Math::Matrix m = Math::Matrix::CreateIdentity();
	ZenConvert::VobObjectInfo ocb;
	ocb.worldMatrix = m;
	ocb.color = Math::float4(1,1,1,1);
	pObjectBuffer->Init(&ocb, sizeof(ZenConvert::VobObjectInfo), sizeof(ZenConvert::VobObjectInfo), RAPI::EBindFlags::B_CONSTANTBUFFER, RAPI::U_DYNAMIC, RAPI::CA_WRITE);

	size_t eIdx = 0;
	for(auto& s : packedMesh.subMeshes)
	{
		if(s.indices.empty())
			continue;

		// Build triangle mesh // TODO: Use indexed vertices
		std::vector<WorldVertex> vxs;
		for(size_t i = 0; i < s.indices.size(); i += 3)
		{
			vxs.push_back(*reinterpret_cast<const WorldVertex*>(&packedMesh.vertices[s.indices[i]]));
			vxs.push_back(*reinterpret_cast<const WorldVertex*>(&packedMesh.vertices[s.indices[i+1]]));
			vxs.push_back(*reinterpret_cast<const WorldVertex*>(&packedMesh.vertices[s.indices[i+2]]));
		}

		RAPI::RBuffer* b = RAPI::REngine::ResourceCache->CreateResource<RAPI::RBuffer>();
		b->Init(vxs.data(), vxs.size() * sizeof(Renderer::WorldVertex), sizeof(Renderer::WorldVertex));

		sm.SetVertexBuffer(0, b);
		sm.SetTexture(0, loadTexture(s.material.texture, vdfs), RAPI::ST_PIXEL);

		// Make entities
		Engine::ObjectHandle e = handles[eIdx];
		

		Engine::Entity* entity = factory.storage().getEntity(e);
		Engine::Components::Visual* visual = factory.storage().addComponent<Engine::Components::Visual>(e);

		if(visual)
		{
			visual->pObjectBuffer = pObjectBuffer;
			visual->pPipelineState = sm.MakeDrawCall(vxs.size());
			entity->setWorldTransform(Math::Matrix::CreateIdentity());
		}

		eIdx++;
	}
}
