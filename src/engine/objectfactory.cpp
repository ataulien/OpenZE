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

#include "renderer/vertextypes.h"

extern RAPI::RBuffer* MakeBox(float extends);
extern RAPI::RBuffer* loadZENMesh(const std::string& file, float scale, std::vector<Math::float3>& zenVertices, std::vector<uint32_t>& zenIndices);
#endif

void Engine::ObjectFactory::test_createObjects()
{
    ObjectHandle handle = m_Storage.createEntity();
    Components::Collision *pCollision = m_Storage.addComponent<Components::Collision>(handle);
    Physics::CollisionShape shape(new btStaticPlaneShape(btVector3(0.0f, 1.0f, 0.0f), 1.0f));
    pCollision->rigidBody.initPhysics(m_pEngine->physicsSystem(), shape,  Math::float3(0.0f, -100.0f, 0.0f));
    pCollision->rigidBody.setFriction(1.0f);
    pCollision->rigidBody.setRestitution(0.1f);

#ifdef ZE_GAME
    RAPI::RPixelShader* ps = RAPI::REngine::ResourceCache->GetCachedObject<RAPI::RPixelShader>("simplePS");
    RAPI::RVertexShader* vs = RAPI::REngine::ResourceCache->GetCachedObject<RAPI::RVertexShader>("simpleVS");
    RAPI::RStateMachine& sm = RAPI::REngine::RenderingDevice->GetStateMachine();
    RAPI::RBuffer *b = MakeBox(1.0f);

    RAPI::RInputLayout* inputLayout = RAPI::RTools::CreateInputLayoutFor<Renderer::WorldVertex>(vs);

    RAPI::RSamplerState* ss;
    RAPI::RTools::MakeDefaultStates(nullptr, &ss, nullptr, nullptr);

    handle = m_Storage.createEntity();
    pCollision = m_Storage.addComponent<Components::Collision>(handle);

    std::vector<Math::float3> zenVertices;
    std::vector<uint32_t> zenIndices;
    RAPI::RBuffer* worldMesh = loadZENMesh("newworld.zen", 1.0f / 50.0f, zenVertices, zenIndices);

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

    Physics::CollisionShape cShape(new btStaticPlaneShape(btVector3(0.0f, 1.0f, 0.0f), 1.0f));
    pCollision->rigidBody.initPhysics(m_pEngine->physicsSystem(), cShape, Math::float3(0.0f, -1.0f, 0.0f));
    pCollision->rigidBody.setRestitution(0.1f);
    pCollision->rigidBody.setFriction(1.0f);

#if 0
    const int n = 1;
    const float turns = 5;
    const float radius = 4.0f;
    const float heightmod = 0.7f;
    for(int i = 0; i < n; i++)
    {
        float p = (static_cast<float>(i) / static_cast<float>(n)) * R_PI * turns;
        btVector3 s = btVector3(sinf(p) * radius, 50.0f + static_cast<float>(i) * heightmod, cosf(p) * radius);

        entity = createEntity();
        m_Mask[entity] = static_cast<EComponents>(C_COLLISION | C_VISUAL);
        m_Collision[entity].pCollisionShape = new btBoxShape(btVector3(1.0f, 1.0f, 1.0f));
        m_Collision[entity].pMotionState = new btDefaultMotionState(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), s));
        btVector3 inertia;
        float mass = 500.0f;
        m_Collision[entity].pCollisionShape->calculateLocalInertia(mass, inertia);
        m_Collision[entity].pRigidBody = new btRigidBody(mass, m_Collision[entity].pMotionState,
                                                         m_Collision[entity].pCollisionShape, inertia);
        m_Collision[entity].pRigidBody->setRestitution(0.1f);
        m_Collision[entity].pRigidBody->setFriction(1.0f);
        m_pEngine->physicsSystem()->addRigidBody(m_Collision[entity].pRigidBody);

        m_Visual[entity].pObjectBuffer = RAPI::REngine::ResourceCache->CreateResource<RAPI::RBuffer>();
        m_Visual[entity].pObjectBuffer->Init(nullptr, sizeof(Math::Matrix), sizeof(Math::Matrix), RAPI::EBindFlags::B_CONSTANTBUFFER, RAPI::EUsageFlags::U_DYNAMIC, RAPI::ECPUAccessFlags::CA_WRITE);

        sm.SetVertexBuffer(0, b);
        sm.SetPixelShader(ps);
        sm.SetVertexShader(vs);
        sm.SetInputLayout(inputLayout);
        sm.SetConstantBuffer(0, m_Visual[entity].pObjectBuffer, RAPI::EShaderType::ST_VERTEX);

        m_Visual[entity].pPipelineState = sm.MakeDrawCall(b->GetSizeInBytes() / b->GetStructuredByteSize());
    }
#endif
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

    RAPI::RInputLayout* inputLayout = RAPI::RTools::CreateInputLayoutFor<Renderer::SimpleVertex>(vs);

    Components::Visual *pVisual = m_Storage.addComponent<Components::Visual>(handle);
    if(!pVisual)
    {
        LogWarn() << "Could not create visual";
        return;
    }
    pVisual->pObjectBuffer = RAPI::REngine::ResourceCache->CreateResource<RAPI::RBuffer>();
    pVisual->pObjectBuffer->Init(nullptr, sizeof(Math::Matrix), sizeof(Math::Matrix), RAPI::EBindFlags::B_CONSTANTBUFFER, RAPI::EUsageFlags::U_DYNAMIC, RAPI::ECPUAccessFlags::CA_WRITE);
    sm.SetVertexBuffer(0, b);
    sm.SetPixelShader(ps);
    sm.SetVertexShader(vs);
    sm.SetInputLayout(inputLayout);
    sm.SetConstantBuffer(0, pVisual->pObjectBuffer, RAPI::EShaderType::ST_VERTEX);

    pVisual->pPipelineState = sm.MakeDrawCall(b->GetSizeInBytes() / b->GetStructuredByteSize());
#endif

    Components::Collision *pCollision = m_Storage.addComponent<Components::Collision>(handle);
    if(!pCollision)
    {
        LogWarn() << "Could not create collision";
        return;
    }
    Physics::CollisionShape collisionShape(new btBoxShape(btVector3(1.0f, 1.0f, 1.0f)));
    LogInfo() << "Init physics";
    pCollision->rigidBody.initPhysics(m_pEngine->physicsSystem(), collisionShape, position, 500.0f);
    pCollision->rigidBody.setFriction(1.0f);
    pCollision->rigidBody.setRestitution(0.1f);
    pCollision->rigidBody.applyCentralImpulse(impulse);
}

void Engine::ObjectFactory::cleanUp()
{
    m_Storage.cleanUp();
}

