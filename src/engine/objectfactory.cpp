#include "objectfactory.h"
#include "engine.h"
#include "utils/vector.h"
#include "utils/mathlib.h"
#include "physics/rigidbody.h"

#ifdef ZE_GAME
#include <REngine.h>
#include <RResourceCache.h>
#include <RBuffer.h>
#include <RPipelineState.h>
#include <RDevice.h>
#include <RStateMachine.h>
#include <RVertexShader.h>
#include <RPixelShader.h>
#include <RInputLayout.h>
#include <RTools.h>
#include <RTexture.h>

#include "game/gameengine.h"
#include "renderer/vertextypes.h"
#include "renderer/visualLoad.h"

extern RAPI::RBuffer* MakeBox(float extends);
#endif

void Engine::ObjectFactory::test_createObjects()
{
	return;
    //Groundplane
    ObjectHandle planeHandle = m_Storage.createEntity();
    Components::Collision *pPlaneCollision = m_Storage.addComponent<Components::Collision>(planeHandle);
    Physics::CollisionShape shape(new btStaticPlaneShape(btVector3(0.0f, 1.0f, 0.0f), 0.0f));
    pPlaneCollision->rigidBody.initPhysics(m_pEngine->physicsSystem(), shape,  Math::float3(0.0f, -500.0f, 0.0f));
    pPlaneCollision->rigidBody.setFriction(1.0f);
    pPlaneCollision->rigidBody.setRestitution(0.1f);

#ifdef ZE_GAME
    RAPI::RPixelShader* ps = RAPI::REngine::ResourceCache->GetCachedObject<RAPI::RPixelShader>("simplePS");
    RAPI::RVertexShader* vs = RAPI::REngine::ResourceCache->GetCachedObject<RAPI::RVertexShader>("simpleVS");
    RAPI::RStateMachine& sm = RAPI::REngine::RenderingDevice->GetStateMachine();
    RAPI::RBuffer *b = MakeBox(1.0f);

    RAPI::RInputLayout* inputLayout = RAPI::RTools::CreateInputLayoutFor<Renderer::WorldVertexInstanced>(vs);

    RAPI::RSamplerState* ss;
    RAPI::RTools::MakeDefaultStates(nullptr, &ss, nullptr, nullptr);

    ObjectHandle handle = m_Storage.createEntity();
    Components::Collision *pCollision = m_Storage.addComponent<Components::Collision>(handle);

    std::vector<Math::float3> zenVertices;
    std::vector<uint32_t> zenIndices;
    RAPI::RBuffer* worldMesh = loadZENMesh("newworld.zen", zenVertices, zenIndices);

    RAPI::RTexture* tx = RAPI::REngine::ResourceCache->GetCachedObject<RAPI::RTexture>("testtexture");
    sm.SetTexture(0, tx, RAPI::ST_PIXEL);
    sm.SetSamplerState(ss);

    btTriangleMesh* wm = new btTriangleMesh;

    for(size_t i = 0; i < zenIndices.size(); i+=3)
    {
        btVector3 v[] = {{zenVertices[zenIndices[i]].x, zenVertices[zenIndices[i]].y, zenVertices[zenIndices[i]].z},
                         {zenVertices[zenIndices[i+1]].x, zenVertices[zenIndices[i+1]].y, zenVertices[zenIndices[i+1]].z},
                         {zenVertices[zenIndices[i+2]].x, zenVertices[zenIndices[i+2]].y, zenVertices[zenIndices[i+2]].z}};
        wm->addTriangle(v[0], v[1], v[2]);
    }

    return;
    Physics::CollisionShape cShape(new btBvhTriangleMeshShape(wm, false));
    pCollision->rigidBody.initPhysics(m_pEngine->physicsSystem(), cShape, Math::float3(0.0f, 0.0f, 0.0f));
    pCollision->rigidBody.setRestitution(0.1f);
    pCollision->rigidBody.setFriction(1.0f);
#endif
}

void Engine::ObjectFactory::test_createPhysicsEntity(const Math::float3 &position, const Math::float3 &impulse)
{
    ObjectHandle handle = m_Storage.createEntity();

#ifdef ZE_GAME
    RAPI::RPixelShader* ps = RAPI::REngine::ResourceCache->GetCachedObject<RAPI::RPixelShader>("simplePS");
    RAPI::RVertexShader* vs = RAPI::REngine::ResourceCache->GetCachedObject<RAPI::RVertexShader>("simpleVS");
    RAPI::RStateMachine& sm = RAPI::REngine::RenderingDevice->GetStateMachine();
    RAPI::RBuffer *b = MakeBox(1.0f);

    RAPI::RInputLayout* inputLayout = RAPI::RTools::CreateInputLayoutFor<Renderer::WorldVertexInstanced>(vs);

    Components::Visual *pVisual = m_Storage.addComponent<Components::Visual>(handle);
    if(!pVisual)
    {
        LogWarn() << "Could not create visual";
        return;
    }
    pVisual->pObjectBuffer = RAPI::REngine::ResourceCache->CreateResource<RAPI::RBuffer>();
	
	ZenConvert::VobObjectInfo ocb;
	ocb.color = Math::float4(1.0f,1.0f,1.0f,1.0f);
	pVisual->pObjectBuffer->Init(&ocb, sizeof(ZenConvert::VobObjectInfo), sizeof(ZenConvert::VobObjectInfo), RAPI::EBindFlags::B_CONSTANTBUFFER, RAPI::U_DYNAMIC, RAPI::CA_WRITE);    sm.SetVertexBuffer(0, b);
    sm.SetPrimitiveTopology(RAPI::PT_TRIANGLE_LIST);
	sm.SetPixelShader(ps);
    sm.SetVertexShader(vs);
    sm.SetInputLayout(inputLayout);
	sm.SetConstantBuffer(0, RAPI::REngine::ResourceCache->GetCachedObject<RAPI::RBuffer>("PerFrameCB"), RAPI::ST_VERTEX);
	sm.SetConstantBuffer(1, pVisual->pObjectBuffer, RAPI::EShaderType::ST_VERTEX);
	sm.SetTexture(0, Renderer::loadTexture("WHITE.TGA", m_pEngine->vdfsFileIndex()), RAPI::ST_PIXEL);
    pVisual->pPipelineState = sm.MakeDrawCall(b->GetSizeInBytes() / b->GetStructuredByteSize());

#endif

    Components::Collision *pCollision = m_Storage.addComponent<Components::Collision>(handle);
    if(!pCollision)
    {
        LogWarn() << "Could not create collision";
        return;
    }
    Physics::CollisionShape collisionShape(new btBoxShape(btVector3(1.0f, 1.0f, 1.0f)));
    pCollision->rigidBody.initPhysics(m_pEngine->physicsSystem(), collisionShape, position, 500.0f);
    pCollision->rigidBody.setFriction(1.0f);
    pCollision->rigidBody.setRestitution(0.1f);
    pCollision->rigidBody.applyCentralImpulse(impulse);
}

void Engine::ObjectFactory::cleanUp()
{
    m_Storage.cleanUp();
}

