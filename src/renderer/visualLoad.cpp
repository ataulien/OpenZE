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

/**
* @brief Loads a texture from the given VDF-Index
*/
RAPI::RTexture* Renderer::loadTexture(const std::string& name, const VDFS::FileIndex& fileIndex)
{
	if(name.empty())
		return nullptr;

	RAPI::RTexture* tx = RAPI::REngine::ResourceCache->GetCachedObject<RAPI::RTexture>(name);
	if(tx)
		return tx;

	// Get from .TGA to -C.TEX
	std::string fileName = name.substr(0, name.find_first_of('.'));
	fileName += "-C.TEX";

	// Read data from vdfs
	std::vector<uint8_t> textureData;
	if(!fileIndex.getFileData(fileName, textureData))
		return nullptr;

	// Convert to actual dds
	std::vector<uint8_t> ddsData;
	ZenConvert::convertZTEX2DDS(textureData, ddsData);

	// Upload texture
	tx = RAPI::REngine::ResourceCache->CreateResource<RAPI::RTexture>();
	tx->CreateTexture(ddsData.data(), ddsData.size(), RAPI::RInt2(0,0), 0, RAPI::TF_FORMAT_UNKNOWN_DXT);

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

	RAPI::RInputLayout* inputLayout = RAPI::RTools::CreateInputLayoutFor<Renderer::WorldVertex>(vs);
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
			vxs.push_back(packedMesh.vertices[s.indices[i]]);
			vxs.push_back(packedMesh.vertices[s.indices[i+1]]);
			vxs.push_back(packedMesh.vertices[s.indices[i+2]]);
		}

		RAPI::RBuffer* b = RAPI::REngine::ResourceCache->CreateResource<RAPI::RBuffer>();
		b->Init(vxs.data(), vxs.size() * sizeof(Renderer::WorldVertex), sizeof(Renderer::WorldVertex));

		sm.SetVertexBuffer(0, b);
		sm.SetTexture(0, loadTexture(s.material.texture, vdfs), RAPI::ST_PIXEL);

		// Make entities
		Engine::ObjectHandle e = handles[eIdx];
		factory.storage().addComponent<Engine::Components::Visual>(e);

		Engine::Components::Visual* visual = factory.storage().getComponent<Engine::Components::Visual>(e);

		if(visual)
		{
			visual->pObjectBuffer = pObjectBuffer;
			visual->pPipelineState = sm.MakeDrawCall(vxs.size());
			visual->tmpWorld = Math::Matrix::CreateIdentity();
		}

		eIdx++;
	}
}
