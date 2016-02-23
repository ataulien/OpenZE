#include "zenWorldMesh.h"
#include "vdfs/fileIndex.h"
#include "vertextypes.h"
#include "zenconvert/zCMesh.h"
#include "utils/logger.h"
#include <RBuffer.h>
#include <REngine.h>
#include <RResourceCache.h>
#include <RDevice.h>
#include <RStateMachine.h>
#include <RTools.h>
#include <RTexture.h>
#include "vdfs/fileIndex.h"
#include "zenconvert/ztex2dds.h"
#include "zenconvert/zCProgMeshProto.h"


static RAPI::RTexture* loadTexture(const std::string& name, VDFS::FileIndex& fileIndex)
{
	RAPI::RTexture* tx = RAPI::REngine::ResourceCache->GetCachedObject<RAPI::RTexture>(name);
	if(tx)
		return tx;

	// Get from .TGA to -C.TEX
	std::string fileName = name.substr(0, name.find_first_of('.'));
	fileName += "-C.TEX";

	std::vector<uint8_t> textureData;
	if(!fileIndex.getFileData(fileName, textureData))
		return nullptr;

	std::vector<uint8_t> ddsData;
	ZenConvert::convertZTEX2DDS(textureData, ddsData);

	tx = RAPI::REngine::ResourceCache->CreateResource<RAPI::RTexture>();
	tx->CreateTexture(ddsData.data(), ddsData.size(), RAPI::RInt2(0,0), 0, RAPI::TF_FORMAT_UNKNOWN_DXT);

	RAPI::REngine::ResourceCache->AddToCache(name, tx);

	return tx;
}

Renderer::ZenWorldMesh::ZenWorldMesh(const ZenConvert::zCMesh & source, float scale, const Math::float3& positionOffset)
{
	std::unordered_map<std::string, std::vector<Renderer::WorldVertex>> verticesByTexture;

	m_VerticesAsTriangles.resize(source.getIndices().size());

	// Get vertices
	for(size_t i = 0, end = m_VerticesAsTriangles.size(); i < end; i++)
	{
		uint32_t idx = source.getIndices()[i];
		uint32_t featidx = source.getFeatureIndices()[i];

		// get vertex and scale
		m_VerticesAsTriangles[i].Position = source.getVertices()[idx];
		m_VerticesAsTriangles[i].Position *= scale;
		m_VerticesAsTriangles[i].Position += positionOffset;

		if(idx < source.getFeatures().size())
		{
			m_VerticesAsTriangles[i].Color = source.getFeatures()[idx].lightStat;
			m_VerticesAsTriangles[i].TexCoord = Math::float2(source.getFeatures()[featidx].uv[0], source.getFeatures()[featidx].uv[1]);
			m_VerticesAsTriangles[i].Normal = source.getFeatures()[idx].vertNormal;
		}
	}

	// Compute normals and resize
	for(size_t i = 0, end = m_VerticesAsTriangles.size(); i < end; i += 3)
	{
		Math::float3 v0 = m_VerticesAsTriangles[i].Position;
		Math::float3 v1 = m_VerticesAsTriangles[i + 1].Position;
		Math::float3 v2 = m_VerticesAsTriangles[i + 2].Position;

		Math::float3 nrm = Math::float3::cross(v1 - v0, v2 - v0).normalize();

		m_VerticesAsTriangles[i+0].Normal = nrm;
		m_VerticesAsTriangles[i+1].Normal = nrm;
		m_VerticesAsTriangles[i+2].Normal = nrm;		

		// Get material info
		const ZenConvert::MaterialInfo& info = source.getMaterials()[source.getTriangleMaterialIndices()[i / 3]];

		for(size_t j = 0; j < 3; j++)
			verticesByTexture[info.texture].emplace_back(m_VerticesAsTriangles[i+j]);
	}

	// Create buffers and states for each texture
	RAPI::RPixelShader* ps = RAPI::REngine::ResourceCache->GetCachedObject<RAPI::RPixelShader>("simplePS");
	RAPI::RVertexShader* vs = RAPI::REngine::ResourceCache->GetCachedObject<RAPI::RVertexShader>("simpleVS");
	RAPI::RStateMachine& sm = RAPI::REngine::RenderingDevice->GetStateMachine();

	RAPI::RInputLayout* inputLayout = RAPI::RTools::CreateInputLayoutFor<Renderer::WorldVertex>(vs);
	RAPI::RSamplerState* ss;
	RAPI::RTools::MakeDefaultStates(nullptr, &ss, nullptr, nullptr);
	sm.SetSamplerState(ss);

	sm.SetPixelShader(ps);
	sm.SetVertexShader(vs);
	sm.SetInputLayout(inputLayout);

	m_pObjectBuffer = RAPI::REngine::ResourceCache->CreateResource<RAPI::RBuffer>();
	sm.SetConstantBuffer(0, m_pObjectBuffer, RAPI::ST_VERTEX);

	Math::Matrix m = Math::Matrix::CreateIdentity();
	m_pObjectBuffer->Init(&m, sizeof(Math::Matrix), sizeof(Math::Matrix), RAPI::EBindFlags::B_CONSTANTBUFFER, RAPI::U_DYNAMIC, RAPI::CA_WRITE);

	static VDFS::FileIndex vdfsIndex;
	vdfsIndex.loadVDF("textures.vdf");
	for(auto& t : verticesByTexture)
	{
		RAPI::RBuffer* b = RAPI::REngine::ResourceCache->CreateResource<RAPI::RBuffer>();
		b->Init(t.second.data(), t.second.size() * sizeof(Renderer::WorldVertex), sizeof(Renderer::WorldVertex));

		m_BufferMap[t.first] = b;

		sm.SetVertexBuffer(0, b);
		sm.SetTexture(0, loadTexture(t.first, vdfsIndex), RAPI::ST_PIXEL);

		// Make drawcalls
		SubMesh subMesh;
		subMesh.state = sm.MakeDrawCall(t.second.size());
		m_SubMeshes.emplace_back(subMesh);
	}
}

Renderer::ZenWorldMesh::ZenWorldMesh(const ZenConvert::zCProgMeshProto& source, float scale, const Math::float3& positionOffset)
{
	std::vector<WorldVertex> vertices;
	std::vector<uint32_t> subMeshIndexOffsets;
	std::unordered_map<std::string, std::vector<Renderer::WorldVertex>> verticesByTexture;
	for(size_t i = 0; i < source.getNumSubmeshes(); i++)
	{
		auto& m = source.getSubmesh(i);

		uint32_t iOff = vertices.size();
		subMeshIndexOffsets.emplace_back(iOff);

		// Get data
		for(int i = 0; i < m.m_WedgeList.size(); i++)
		{
			const ZenConvert::zWedge& wedge = m.m_WedgeList[i];

			WorldVertex wv;
			wv.Position = source.getPositionList()[wedge.m_VertexIndex] * scale + positionOffset;
			wv.Normal = wedge.m_Normal;
			wv.TexCoord = wedge.m_Texcoord;
			wv.Color = 0xFFFFFFFF; // TODO: Apply color from material!
			vertices.emplace_back(wv);
		}		
	}

	for(size_t i = 0; i < source.getNumSubmeshes(); i++)
	{
		auto& m = source.getSubmesh(i);
		auto& vxs = verticesByTexture[m.m_Material.texture];

		for(uint32_t t = 0; t < m.m_TriangleList.size(); t++)
		{
			for(uint32_t j = 0; j < 3; j++)
			{
				vxs.push_back( vertices[m.m_TriangleList[t].m_Wedges[j] + subMeshIndexOffsets[i]]);
			}			
		}
	}


	// Create buffers and states for each texture
	RAPI::RPixelShader* ps = RAPI::REngine::ResourceCache->GetCachedObject<RAPI::RPixelShader>("simplePS");
	RAPI::RVertexShader* vs = RAPI::REngine::ResourceCache->GetCachedObject<RAPI::RVertexShader>("simpleVS");
	RAPI::RStateMachine& sm = RAPI::REngine::RenderingDevice->GetStateMachine();

	RAPI::RInputLayout* inputLayout = RAPI::RTools::CreateInputLayoutFor<Renderer::WorldVertex>(vs);
	RAPI::RSamplerState* ss;
	RAPI::RTools::MakeDefaultStates(nullptr, &ss, nullptr, nullptr);
	sm.SetSamplerState(ss);

	sm.SetPixelShader(ps);
	sm.SetVertexShader(vs);
	sm.SetInputLayout(inputLayout);

	m_pObjectBuffer = RAPI::REngine::ResourceCache->CreateResource<RAPI::RBuffer>();
	sm.SetConstantBuffer(0, m_pObjectBuffer, RAPI::ST_VERTEX);

	Math::Matrix m = Math::Matrix::CreateIdentity();
	m_pObjectBuffer->Init(&m, sizeof(Math::Matrix), sizeof(Math::Matrix), RAPI::EBindFlags::B_CONSTANTBUFFER, RAPI::U_DYNAMIC, RAPI::CA_WRITE);

	static VDFS::FileIndex vdfsIndex;
	vdfsIndex.loadVDF("textures.vdf");

	for(auto& t : verticesByTexture)
	{
		RAPI::RBuffer* b = RAPI::REngine::ResourceCache->CreateResource<RAPI::RBuffer>();
		b->Init(t.second.data(), t.second.size() * sizeof(Renderer::WorldVertex), sizeof(Renderer::WorldVertex));

		m_BufferMap[t.first] = b;

		sm.SetVertexBuffer(0, b);
		sm.SetTexture(0, loadTexture(t.first, vdfsIndex), RAPI::ST_PIXEL);

		// Make drawcalls
		SubMesh subMesh;
		subMesh.state = sm.MakeDrawCall(t.second.size());
		m_SubMeshes.emplace_back(subMesh);
	}
}

Renderer::ZenWorldMesh::~ZenWorldMesh()
{
	for(auto& b : m_BufferMap)
	{
		RAPI::REngine::ResourceCache->DeleteResource(b.second);
	}
}

void Renderer::ZenWorldMesh::render(const Math::Matrix& viewProj, RAPI::RRenderQueueID queue)
{
	m_pObjectBuffer->UpdateData(&viewProj);

	for(size_t i = 0, end = m_SubMeshes.size(); i < end; ++i)
	{
		RAPI::REngine::RenderingDevice->QueuePipelineState(m_SubMeshes[i].state, queue);		
	}
}
