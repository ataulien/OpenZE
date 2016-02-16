#include "objectfactory.h"
#include "physics/rigidbody.h"
#include "engine.h"

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

#include "renderer/vertextypes.h"

extern RAPI::RBuffer* MakeBox(float extends);
extern RAPI::RBuffer* loadZENMesh(const std::string& file, float scale, std::vector<Math::float3>& zenVertices, std::vector<uint32_t>& zenIndices);
#endif

Engine::ObjectFactory::ObjectFactory(Engine *pEngine, uint32_t objCount) :
    m_pEngine(pEngine),
    m_Mask(objCount, static_cast<EComponents>(0)),
    m_Collision(objCount),
    #ifdef ZE_GAME
    m_Visual(objCount),
    #endif
    m_Attributes(objCount),
    m_AI(objCount)
{
}

uint32_t Engine::ObjectFactory::createEntity()
{
    uint32_t entity;
    for(entity = 0; entity < m_Mask.size(); ++entity)
    {
        if(m_Mask[entity] == C_NONE)
            return entity;
    }

    m_Mask.push_back(C_NONE);
    m_Collision.push_back(Components::Collision());
#ifdef ZE_GAME
    m_Visual.push_back(Components::Visual());
#endif
    m_Attributes.push_back(Components::Attributes());
    m_AI.push_back(Components::AI());

    return entity;
}

void Engine::ObjectFactory::destroyEntity(uint32_t entity)
{
    m_Mask[entity] = C_NONE;
}

uint32_t Engine::ObjectFactory::createObject()
{
    uint32_t entity = createEntity();

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

	RAPI::RInputLayout* inputLayout = RAPI::RTools::CreateInputLayoutFor<Renderer::SimpleVertex>(vs);


	entity = createEntity();
	m_Mask[entity] = static_cast<EComponents>(C_COLLISION | C_VISUAL);

	std::vector<Math::float3> zenVertices;
	std::vector<uint32_t> zenIndices;
	RAPI::RBuffer* worldMesh = loadZENMesh("NEWWORLD_ASCII.zen", 1.0f / 50.0f, zenVertices, zenIndices);

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

	m_Visual[entity].pObjectBuffer = RAPI::REngine::ResourceCache->CreateResource<RAPI::RBuffer>();

	Math::Matrix m = Math::Matrix::CreateIdentity();
	m_Visual[entity].pObjectBuffer->Init(&m, sizeof(Math::Matrix), sizeof(Math::Matrix), RAPI::EBindFlags::B_CONSTANTBUFFER, RAPI::EUsageFlags::U_DYNAMIC, RAPI::ECPUAccessFlags::CA_WRITE);

	sm.SetVertexBuffer(0, worldMesh);
	sm.SetPixelShader(ps);
	sm.SetVertexShader(vs);
	sm.SetInputLayout(inputLayout);
	sm.SetConstantBuffer(0, m_Visual[entity].pObjectBuffer, RAPI::EShaderType::ST_VERTEX);

	m_Visual[entity].pPipelineState = sm.MakeDrawCall(worldMesh->GetSizeInBytes() / worldMesh->GetStructuredByteSize());

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

    return entity;
}

uint32_t Engine::ObjectFactory::test_createPhysicsEntity(const Math::float3& position, const Math::float3& impulse)
{
	uint32_t entity = createEntity();

	RAPI::RPixelShader* ps = RAPI::REngine::ResourceCache->GetCachedObject<RAPI::RPixelShader>("simplePS");
	RAPI::RVertexShader* vs = RAPI::REngine::ResourceCache->GetCachedObject<RAPI::RVertexShader>("simpleVS");
	RAPI::RStateMachine& sm = RAPI::REngine::RenderingDevice->GetStateMachine();
	RAPI::RBuffer *b = MakeBox(1.0f);

	RAPI::RInputLayout* inputLayout = RAPI::RTools::CreateInputLayoutFor<Renderer::SimpleVertex>(vs);

	btVector3 s = btVector3(position.x, position.y, position.z);

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
	m_Collision[entity].pRigidBody->applyCentralImpulse( btVector3(impulse.x, impulse.y, impulse.z));
	m_pEngine->physicsSystem()->addRigidBody(m_Collision[entity].pRigidBody);

	m_Visual[entity].pObjectBuffer = RAPI::REngine::ResourceCache->CreateResource<RAPI::RBuffer>();
	m_Visual[entity].pObjectBuffer->Init(nullptr, sizeof(Math::Matrix), sizeof(Math::Matrix), RAPI::EBindFlags::B_CONSTANTBUFFER, RAPI::EUsageFlags::U_DYNAMIC, RAPI::ECPUAccessFlags::CA_WRITE);

	sm.SetVertexBuffer(0, b);
	sm.SetPixelShader(ps);
	sm.SetVertexShader(vs);
	sm.SetInputLayout(inputLayout);
	sm.SetConstantBuffer(0, m_Visual[entity].pObjectBuffer, RAPI::EShaderType::ST_VERTEX);

	m_Visual[entity].pPipelineState = sm.MakeDrawCall(b->GetSizeInBytes() / b->GetStructuredByteSize());

	return entity;
}