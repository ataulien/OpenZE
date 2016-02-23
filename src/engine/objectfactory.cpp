#include "objectfactory.h"
#include "physics/rigidbody.h"
#include "engine.h"
#include "utils/mathlib.h"

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

Engine::ObjectFactory::ObjectFactory(Engine *pEngine, uint32_t initialSize) :
    m_pEngine(pEngine),
    m_Entities(initialSize),
    m_CCollisions(initialSize),
    #ifdef ZE_GAME
    m_CVisuals(initialSize),
    #endif
    m_CAttributes(initialSize),
    m_CAis(initialSize)
{
}

Engine::ObjectFactory::~ObjectFactory()
{
}

Engine::ObjectHandle Engine::ObjectFactory::createEntity()
{
    uint32_t offset = firstFreeEntityOffset();
    ObjectHandle handle;
    if(offset == static_cast<uint32_t>(-1))
    {
        handle.handle = 0;
        return handle;
    }

    handle.offset = offset;
    handle.count = ++m_Entities[handle.offset].handle.count;
    return handle;
}

void Engine::ObjectFactory::destroyEntity(uint32_t offset)
{
     if(m_Entities.size() <= offset)
        return;

    Entity &entity = m_Entities[offset];

    if((entity.mask & C_COLLISION) == C_COLLISION)
        m_CCollisions[offset].cleanUp();

#ifdef ZE_GAME
    if((entity.mask & C_VISUAL) == C_VISUAL)
        m_CVisuals[offset].cleanUp();
#endif

    entity.mask = C_NONE;
}

void Engine::ObjectFactory::destroyEntity(ObjectHandle handle)
{
    if(m_Entities.size() <= handle.offset)
        return;

    Entity &entity = m_Entities[handle.offset];

    if((entity.mask & C_COLLISION) == C_COLLISION)
        m_CCollisions[handle.offset].cleanUp();

#ifdef ZE_GAME
    if((entity.mask & C_VISUAL) == C_VISUAL)
        m_CVisuals[handle.offset].cleanUp();
#endif

    entity.mask = C_NONE;
}

void Engine::ObjectFactory::test_createObjects()
{
#if 0
    ObjectHandle handle = createEntity();
    m_Mask[entity] = static_cast<EComponents>(C_COLLISION);
    m_Collision[entity].pCollisionShape = new btStaticPlaneShape(btVector3(0.0f, 1.0f, 0.0f), 1.0f);
    m_Collision[entity].pMotionState = new btDefaultMotionState(btTransform(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), btVector3(0.0f, -100.0f, 0.0f))));
    m_Collision[entity].pRigidBody = new btRigidBody(0.0f, m_Collision[entity].pMotionState, m_Collision[entity].pCollisionShape);
    m_pEngine->physicsSystem()->addRigidBody(m_Collision[entity].pRigidBody);
    m_Collision[entity].pRigidBody->setRestitution(0.1f);
    m_Collision[entity].pRigidBody->setFriction(1.0f);

#ifdef ZE_GAME
    RAPI::RPixelShader* ps = RAPI::REngine::ResourceCache->GetCachedObject<RAPI::RPixelShader>("simplePS");
    RAPI::RVertexShader* vs = RAPI::REngine::ResourceCache->GetCachedObject<RAPI::RVertexShader>("simpleVS");
    RAPI::RStateMachine& sm = RAPI::REngine::RenderingDevice->GetStateMachine();
    RAPI::RBuffer *b = MakeBox(1.0f);

	RAPI::RInputLayout* inputLayout = RAPI::RTools::CreateInputLayoutFor<Renderer::WorldVertex>(vs);

	RAPI::RSamplerState* ss;
	RAPI::RTools::MakeDefaultStates(nullptr, &ss, nullptr, nullptr);

	entity = createEntity();
    m_Mask[entity] = static_cast<EComponents>(C_COLLISION);

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

    m_Collision[entity].pCollisionShape = new btBvhTriangleMeshShape(wm, true, true);
    m_Collision[entity].pMotionState = new btDefaultMotionState(btTransform(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f), btVector3(0.0f, -1.0f, 0.0f))));
    m_Collision[entity].pRigidBody = new btRigidBody(0.0f, m_Collision[entity].pMotionState, m_Collision[entity].pCollisionShape);
    m_pEngine->physicsSystem()->addRigidBody(m_Collision[entity].pRigidBody);
    m_Collision[entity].pRigidBody->setRestitution(0.1f);
    m_Collision[entity].pRigidBody->setFriction(1.0f);

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

Utils::Vector<Engine::Entity> &Engine::ObjectFactory::getEntities()
{
    return m_Entities;
}

Engine::Components::Collision *Engine::ObjectFactory::getCollision(const ObjectHandle &handle)
{
    if(handle.offset >= m_Entities.size() || m_Entities[handle.offset].handle.count != handle.count)
        return nullptr;

    return &m_CCollisions[handle.offset];
}

Engine::Components::Collision *Engine::ObjectFactory::addCollisionComponent(const ObjectHandle &handle)
{
    if(handle.offset >= m_Entities.size())
        return nullptr;

    Entity &entity = m_Entities[handle.offset];
    if(m_Entities[handle.offset].handle.count != handle.count)
        return nullptr;

    if((entity.mask & C_COLLISION) == C_COLLISION)
        return nullptr;

    entity.mask = static_cast<EComponents>(entity.mask | C_COLLISION);
    return &m_CCollisions[handle.offset];
}

bool Engine::ObjectFactory::removeCollisionComponent(const ObjectHandle &handle)
{
    Entity &entity = m_Entities[handle.offset];
    if(handle.offset >= m_Entities.size() || entity.handle.count != handle.count)
        return false;

    if((entity.mask & C_COLLISION) != C_COLLISION)
        return false;

    m_CCollisions[handle.offset].cleanUp();
    entity.mask = static_cast<EComponents>(entity.mask | ~C_COLLISION);

    return true;
}

#ifdef ZE_GAME
Engine::Components::Visual *Engine::ObjectFactory::getVisual(const ObjectHandle &handle)
{
    if(handle.offset >= m_Entities.size() || m_Entities[handle.offset].handle.count != handle.count)
        return nullptr;

    return &m_CVisuals[handle.offset];
}

Engine::Components::Visual *Engine::ObjectFactory::addVisualComponent(const ObjectHandle &handle)
{
    if(handle.offset >= m_Entities.size())
        return nullptr;

    Entity &entity = m_Entities[handle.offset];
    if(m_Entities[handle.offset].handle.count != handle.count)
        return nullptr;

    if((entity.mask & C_VISUAL) == C_VISUAL)
        return nullptr;

    entity.mask = static_cast<EComponents>(entity.mask | C_VISUAL);
    return &m_CVisuals[handle.offset];
}

bool Engine::ObjectFactory::removeVisualComponent(const ObjectHandle &handle)
{
    Entity &entity = m_Entities[handle.offset];
    if(handle.offset >= m_Entities.size() || entity.handle.count != handle.count)
        return false;

    if((entity.mask & C_VISUAL) != C_VISUAL)
        return false;

    m_CVisuals[handle.offset].cleanUp();
    entity.mask = static_cast<EComponents>(entity.mask | ~C_VISUAL);

    return true;
}
#endif

void Engine::ObjectFactory::cleanUp()
{
    for(uint32_t i = 0, end = m_Entities.size(); i < end; ++i)
        destroyEntity(m_Entities[i].handle);
}

uint32_t Engine::ObjectFactory::firstFreeEntityOffset()
{
    uint32_t offset, end;
    for(offset = 0, end = m_Entities.size(); offset < end; ++offset)
    {
        if(m_Entities.m_Data[offset].mask == C_NONE)
            return offset;
    }

    if(m_Entities.size() >= Math::ipow(2, OBJECT_HANDLE_HANDLE_BITS))
        return static_cast<uint32_t>(-1);

    m_Entities.m_Data.emplace_back(m_Entities.size());
    m_CCollisions.m_Data.emplace_back();
#ifdef ZE_GAME
    m_CVisuals.m_Data.emplace_back();
#endif
    m_CAttributes.m_Data.emplace_back();
    m_CAis.m_Data.emplace_back();

    LogInfo() << ": object count: " << m_Entities.size();
    return offset;
}

void Engine::ObjectFactory::test_createPhysicsEntity(const Math::float3& position, const Math::float3& impulse)
{
    static uint32_t count = 0;
	LogInfo() << ": count: " << count << std::endl;
    if(count % 100 == 0 && count != 0)
    {
		LogInfo() << ": destroy entity 100" << std::endl;
        destroyEntity(100);
    }
    ++count;
    ObjectHandle handle = createEntity();

    btVector3 s = btVector3(position.x, position.y, position.z);

#ifdef ZE_GAME
    RAPI::RPixelShader* ps = RAPI::REngine::ResourceCache->GetCachedObject<RAPI::RPixelShader>("simplePS");
    RAPI::RVertexShader* vs = RAPI::REngine::ResourceCache->GetCachedObject<RAPI::RVertexShader>("simpleVS");
    RAPI::RStateMachine& sm = RAPI::REngine::RenderingDevice->GetStateMachine();
    RAPI::RBuffer *b = MakeBox(1.0f);

    RAPI::RInputLayout* inputLayout = RAPI::RTools::CreateInputLayoutFor<Renderer::SimpleVertex>(vs);

    Components::Visual *pVisual = addVisualComponent(handle);
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

    Components::Collision *pCollision = addCollisionComponent(handle);
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

Engine::Entity::Entity(uint32_t offset) :
    mask(C_NONE)
{
    handle.count = 0;
    handle.offset = offset;
}
