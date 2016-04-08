#include <RTools.h>
#include <RBuffer.h>
#include <REngine.h>
#include <RResourceCache.h>
#include <RStateMachine.h>
#include <RPixelShader.h>
#include <RVertexShader.h>
#include <RInputLayout.h>
#include <RDevice.h>

#include "gameengine.h"
#include "utils/mathlib.h"
#include "renderer/vertextypes.h"
#include "gameengine.h"
#include "physics/motionstate.h"
#include "components.h"
#include "components/collision.h"
#include "components/visual.h"
#include "zenconvert/zenParser.h"
#include "zenconvert/vob.h"
#include "zenconvert/zCMesh.h"
#include "vdfs/fileIndex.h"
#include "zenconvert/ztex2dds.h"
#include <RTexture.h>
#include "zenWorld.h"
#include "renderer/renderSystem.h"
#include <thread>
#include "animHandler.h"
#include "shaders.h"

#ifdef RAPI_USE_GLFW
#include <GLFW/glfw3.h>
#include "renderer/GLFW_window.h"
#endif

#include "zenconvert/zCModelAni.h"
#include "zenconvert/zCModelMeshLib.h"

#ifdef RAPI_USE_GLES3
#include "renderer/EGL_window.h"
#endif

#ifdef __ANDROID__
const std::string BASE_DIR = "/sdcard/openZE/";
#else
const std::string BASE_DIR = "";
#endif

#define RENDER_MASK (::Engine::C_VISUAL)


RAPI::RTexture* loadVDFTexture(const std::string& file)
{
	VDFS::FileIndex idx;

	idx.loadVDF("Textures.vdf");

	std::vector<uint8_t> testData;
	idx.getFileData("CHAPTER5_PAL-C.TEX", testData);

	std::vector<uint8_t> ddsData;
	ZenConvert::convertZTEX2DDS(testData, ddsData);

	RAPI::RTexture* tx = RAPI::REngine::ResourceCache->CreateResource<RAPI::RTexture>();
	tx->CreateTexture(ddsData.data(), ddsData.size(), RAPI::RInt2(0, 0), 0, RAPI::TF_FORMAT_UNKNOWN_DXT);

	RAPI::REngine::ResourceCache->AddToCache("testtexture", tx);
	return nullptr;
}

RAPI::RBuffer* loadZENMesh(const std::string& file, std::vector<Math::float3>& zenVertices, std::vector<uint32_t>& zenIndices, float scale)
{
	return nullptr;
	ZenConvert::Chunk parentVob("parent", "", 0);
	ZenConvert::zCMesh worldMesh;

	try
	{
		ZenConvert::ZenParser parser(file);
	}
	catch(std::exception &e)
	{
		LogError() << e.what();
		return nullptr;
	}

	std::vector<Renderer::WorldVertex> vx;
	vx.resize(worldMesh.getIndices().size());

	for(auto& v : worldMesh.getVertices())
	{
		zenVertices.emplace_back(v * scale);
	}

	// Get vertices
	for(size_t i = 0, end = vx.size(); i < end; i++)
	{
		uint32_t idx = worldMesh.getIndices()[i];
		uint32_t featidx = worldMesh.getFeatureIndices()[i];

		if(idx >= worldMesh.getVertices().size())
			LogError() << "asdasd";

		// get vertex and scale
		vx[i].Position = worldMesh.getVertices()[idx];
		vx[i].Position *= scale;

		if(idx < worldMesh.getFeatures().size())
		{
			vx[i].Color = worldMesh.getFeatures()[featidx].lightStat;
			vx[i].TexCoord = Math::float2(worldMesh.getFeatures()[featidx].uv[0], worldMesh.getFeatures()[featidx].uv[1]);
			vx[i].Normal = worldMesh.getFeatures()[featidx].vertNormal;
		}

		zenIndices.emplace_back(idx);
	}

	// Compute normals and resize
	for(size_t i = 0, end = vx.size(); i < end; i += 3)
	{
		Math::float3 v0 = vx[i].Position;
		Math::float3 v1 = vx[i + 1].Position;
		Math::float3 v2 = vx[i + 2].Position;

		Math::float3 nrm = Math::float3::cross(v1 - v0, v2 - v0).normalize();

		vx[i + 0].Normal = nrm;
		vx[i + 1].Normal = nrm;
		vx[i + 2].Normal = nrm;
	}
	return nullptr;
}

RAPI::RBuffer* MakeBox(float extends)
{
	const int n = 36;
	Renderer::WorldVertex vx[n];
	int i = 0;

	vx[i++].Position = Math::float3(-1.0f, -1.0f, -1.0f);
	vx[i++].Position = Math::float3(-1.0f, -1.0f, 1.0f);
	vx[i++].Position = Math::float3(-1.0f, 1.0f, 1.0f);
	vx[i++].Position = Math::float3(1.0f, 1.0f, -1.0f);
	vx[i++].Position = Math::float3(-1.0f, -1.0f, -1.0f);
	vx[i++].Position = Math::float3(-1.0f, 1.0f, -1.0f);
	vx[i++].Position = Math::float3(1.0f, -1.0f, 1.0f);
	vx[i++].Position = Math::float3(-1.0f, -1.0f, -1.0f);
	vx[i++].Position = Math::float3(1.0f, -1.0f, -1.0f);
	vx[i++].Position = Math::float3(1.0f, 1.0f, -1.0f);
	vx[i++].Position = Math::float3(1.0f, -1.0f, -1.0f);
	vx[i++].Position = Math::float3(-1.0f, -1.0f, -1.0f);
	vx[i++].Position = Math::float3(-1.0f, -1.0f, -1.0f);
	vx[i++].Position = Math::float3(-1.0f, 1.0f, 1.0f);
	vx[i++].Position = Math::float3(-1.0f, 1.0f, -1.0f);
	vx[i++].Position = Math::float3(1.0f, -1.0f, 1.0f);
	vx[i++].Position = Math::float3(-1.0f, -1.0f, 1.0f);
	vx[i++].Position = Math::float3(-1.0f, -1.0f, -1.0f);
	vx[i++].Position = Math::float3(-1.0f, 1.0f, 1.0f);
	vx[i++].Position = Math::float3(-1.0f, -1.0f, 1.0f);
	vx[i++].Position = Math::float3(1.0f, -1.0f, 1.0f);
	vx[i++].Position = Math::float3(1.0f, 1.0f, 1.0f);
	vx[i++].Position = Math::float3(1.0f, -1.0f, -1.0f);
	vx[i++].Position = Math::float3(1.0f, 1.0f, -1.0f);
	vx[i++].Position = Math::float3(1.0f, -1.0f, -1.0f);
	vx[i++].Position = Math::float3(1.0f, 1.0f, 1.0f);
	vx[i++].Position = Math::float3(1.0f, -1.0f, 1.0f);
	vx[i++].Position = Math::float3(1.0f, 1.0f, 1.0f);
	vx[i++].Position = Math::float3(1.0f, 1.0f, -1.0f);
	vx[i++].Position = Math::float3(-1.0f, 1.0f, -1.0f);
	vx[i++].Position = Math::float3(1.0f, 1.0f, 1.0f);
	vx[i++].Position = Math::float3(-1.0f, 1.0f, -1.0f);
	vx[i++].Position = Math::float3(-1.0f, 1.0f, 1.0f);
	vx[i++].Position = Math::float3(1.0f, 1.0f, 1.0f);
	vx[i++].Position = Math::float3(-1.0f, 1.0f, 1.0f);
	vx[i++].Position = Math::float3(1.0f, -1.0f, 1.0f);

	for(i = 0; i < n; i++)
	{
		vx[i].TexCoord = Math::float2(vx[i].Position.x * 0.5f + 0.5f, vx[i].Position.z * 0.5f + 0.5f);
	}

	// Generate normals
	for(i = 0; i < n; i += 3)
	{
		std::swap(vx[i + 2].Position, vx[i + 1].Position);

		Math::float3 v0 = vx[i].Position;
		Math::float3 v1 = vx[i + 1].Position;
		Math::float3 v2 = vx[i + 2].Position;

		Math::float3 nrm = Math::float3::cross(v1 - v0, v2 - v0).normalize();

		vx[i + 0].Normal = nrm;
		vx[i + 1].Normal = nrm;
		vx[i + 2].Normal = nrm;
	}

	// Loop through all vertices and apply the extends
	for(i = 0; i < n; i++)
	{
		vx[i].Color = 0xFFFFFFFF;
		vx[i].Position *= extends;
	}

	RAPI::RBuffer* buffer = RAPI::REngine::ResourceCache->CreateResource<RAPI::RBuffer>();
	buffer->Init(&vx[0], sizeof(vx), sizeof(Renderer::WorldVertex));
	return buffer;
}

Engine::GameEngine::GameEngine(int argc, char *argv[], Renderer::Window* window) :
	Engine(argc, argv),
	m_Window(window),
	m_CameraZoom(1.0f),
	m_CameraAngle(0.0f),
	m_CameraCenter(0, 0, 0),
	m_TestWorld(nullptr),
	m_IsFlying(true)
{

}

Engine::GameEngine::~GameEngine()
{
	delete m_TestWorld;
	delete m_pRenderSystem;
	RAPI::REngine::UninitializeEngine();
}

static void GetFirstNMessages(GLuint numMsgs)
{
#ifdef GL_MAX_DEBUG_MESSAGE_LENGTH
	GLint maxMsgLen = 0;
	glGetIntegerv(GL_MAX_DEBUG_MESSAGE_LENGTH, &maxMsgLen);

	std::vector<GLchar> msgData(numMsgs * maxMsgLen);
	std::vector<GLenum> sources(numMsgs);
	std::vector<GLenum> types(numMsgs);
	std::vector<GLenum> severities(numMsgs);
	std::vector<GLuint> ids(numMsgs);
	std::vector<GLsizei> lengths(numMsgs);

	GLuint numFound = glGetDebugMessageLog(numMsgs, (GLsizei)msgData.size(), &sources[0], &types[0], &ids[0], &severities[0], &lengths[0], &msgData[0]);

	sources.resize(numFound);
	types.resize(numFound);
	severities.resize(numFound);
	ids.resize(numFound);
	lengths.resize(numFound);

	std::vector<std::string> messages;
	messages.reserve(numFound);

	std::vector<GLchar>::iterator currPos = msgData.begin();
	for(size_t msg = 0; msg < lengths.size(); ++msg)
	{
		messages.push_back(std::string(currPos, currPos + lengths[msg] - 1));
		currPos = currPos + lengths[msg];

		LogInfo() << messages.back();
	}
#endif
}

static GLint vertices[][3] = {
	{ -0x10000, -0x10000, -0x10000 },
	{  0x10000, -0x10000, -0x10000 },
	{  0x10000,  0x10000, -0x10000 },
	{ -0x10000,  0x10000, -0x10000 },
	{ -0x10000, -0x10000,  0x10000 },
	{  0x10000, -0x10000,  0x10000 },
	{  0x10000,  0x10000,  0x10000 },
	{ -0x10000,  0x10000,  0x10000 }
};

static GLint colors[][4] = {
	{ 0x00000, 0x00000, 0x00000, 0x10000 },
	{ 0x10000, 0x00000, 0x00000, 0x10000 },
	{ 0x10000, 0x10000, 0x00000, 0x10000 },
	{ 0x00000, 0x10000, 0x00000, 0x10000 },
	{ 0x00000, 0x00000, 0x10000, 0x10000 },
	{ 0x10000, 0x00000, 0x10000, 0x10000 },
	{ 0x10000, 0x10000, 0x10000, 0x10000 },
	{ 0x00000, 0x10000, 0x10000, 0x10000 }
};

GLubyte indices[] = {
	0, 4, 5,    0, 5, 1,
	1, 5, 6,    1, 6, 2,
	2, 6, 7,    2, 7, 3,
	3, 7, 4,    3, 4, 0,
	4, 7, 6,    4, 6, 5,
	3, 0, 1,    3, 1, 2
};


bool Engine::GameEngine::render(float alpha)
{
	(void)alpha;

	bool retVal = true;

#ifdef __ANDROID__
	m_IsFlying = false;
#endif

	// Get ground polygon and interpolate lighting
	/*Math::float4 f4Lighting;
	Physics::RayTestResult r = physicsSystem()->rayTest(m_CameraCenter, m_CameraCenter + Math::float3(0,-100,0), Physics::CT_WorldMesh);
	if(r.hitTriangleIndex != UINT_MAX)
	{
		f4Lighting = m_TestWorld->getWorldMesh().getTriangleList()[r.hitTriangleIndex].interpolateLighting(r.hitPosition);
	}*/

	// Update window-title every N seconds
	static double s_TitleUpdateMod = 0;
	const double TITLE_UPDATE_DELAY = 0.2;
	s_TitleUpdateMod += m_MainLoopTimer.getAvgDelta().count();

	if(s_TitleUpdateMod > TITLE_UPDATE_DELAY)
	{
		// Update header
		double frameDeltaSec = m_MainLoopTimer.getAvgDelta().count();
		double frameDeltaMS = m_MainLoopTimer.getAvgDelta().count() * 1000.0f;
		m_Window->setWindowTitle("openZE - FPS: " + std::to_string(1.0 / frameDeltaSec)
			+ " (" + std::to_string(frameDeltaMS) + "ms) Vobs: " + std::to_string(m_Factory.storage().getEntities().size())
			+ " Camera: " + (m_IsFlying ? "Flying" : "Walking"));

		s_TitleUpdateMod = 0.0;
	}

	m_Window->pollEvent([&](Renderer::Window::Event ev)
	{
		switch(ev.EventType)
		{
		case Renderer::Window::EEvent::E_Closed:
			retVal = false;
			break;

		case Renderer::Window::E_Resized:
			LogInfo() << "Resized window!";
			RAPI::REngine::RenderingDevice->OnResize();
			break;

		case Renderer::Window::E_KeyEvent:
			if(ev.KeyboardEvent.action == Renderer::Window::EA_Pressed)
			{
				switch(ev.KeyboardEvent.key)
				{
				case Utils::EKey::KEY_F6:
					m_IsFlying = !m_IsFlying;
					break;

				case Utils::EKey::KEY_F7:
				{
					static bool s_enabled = true;
					s_enabled = !s_enabled;
					RAPI::REngine::RenderingDevice->SetDoDrawCalls(s_enabled);
				}
				break;

				case Utils::EKey::KEY_F8:
					m_Window->switchMode(!m_Window->isFullscreen());
					break;
				}
			}
			break;
		}
	});

	// Grab window-events
	// TODO: Do this in an update-function, not the render-one!
	float MOVEMENT_SPEED = 30.0f * (m_Window->getKeyPressed(Utils::EKey::KEY_LEFT_SHIFT) ? 2.0f : 1.0f) * (m_IsFlying ? 1.0f : 0.25f);
	const float ZOOM_SPEED = 40.0f;
	const float TURN_SPEED = Math::DegToRad(70);
	float movement = MOVEMENT_SPEED * m_MainLoopTimer.getAvgDelta().count();
	float turn = TURN_SPEED * m_MainLoopTimer.getAvgDelta().count();

	// Thumbstick movement
	Math::float2 thumbs[2] = { m_Window->getVirtualThumbstickDirection(0), m_Window->getVirtualThumbstickDirection(1) };
	//m_CameraCenter += -1.3f * thumbs[0].x * (Math::float3::cross(Math::float3(0, 1, 0), Math::float3(sinf(m_CameraAngle), 0, cosf(m_CameraAngle))).normalize() * movement);
	//m_CameraCenter += 1.3f * thumbs[0].y * (Math::float3(sinf(m_CameraAngle), 0, cosf(m_CameraAngle)) * movement);

	//POINT mp; GetCursorPos(&mp); ScreenToClient((HWND)m_Window->getNativeHandle(), &mp);
	//RECT r; GetClientRect((HWND)m_Window->getNativeHandle(), &r);
	//Math::float2 relMP = Math::float2((r.right / 2.0f) - mp.x, (r.bottom / 2.0f) - mp.y).normalize();
	//thumbs[0] = relMP;

	int dirPressed = 0;

	// Map directions to keys
	if(thumbs[0] != Math::float2(0, 0))
	{
		if(thumbs[0].dot(Math::float2(0.0f, 1.0f)) > 0.5f)
			dirPressed = Utils::KEY_S;
		else if(thumbs[0].dot(Math::float2(0.0f, -1.0f)) > 0.5f)
			dirPressed = Utils::KEY_W;
		else if(thumbs[0].dot(Math::float2(-1.0f, 0.0f)) > 0.5f)
			dirPressed = Utils::KEY_A;
		else if(thumbs[0].dot(Math::float2(1.0f, 0.0f)) > 0.5f)
			dirPressed = Utils::KEY_D;
	}

	m_CameraAngle += 1.4f * turn * thumbs[1].x;

	if(m_Window->getKeyPressed(Utils::EKey::KEY_LEFT) || (m_IsFlying ? false : m_Window->getKeyPressed(Utils::EKey::KEY_Q)))
		m_CameraAngle -= turn;

	if(m_Window->getKeyPressed(Utils::EKey::KEY_RIGHT) || (m_IsFlying ? false : m_Window->getKeyPressed(Utils::EKey::KEY_E)))
		m_CameraAngle += turn;

	// If walking, query the current animations speed
	if(!m_IsFlying)
	{
		Entity* e = objectFactory().storage().getEntity(m_TestWorld->getPlayer());
		Components::AnimationController* pAnim = objectFactory().storage().getComponent<Components::AnimationController>(m_TestWorld->getPlayer());
		Components::Visual* pVisual = objectFactory().storage().getComponent<Components::Visual>(m_TestWorld->getPlayer());

		// Replace with the current animations velocity
		float speedMod = (m_Window->getKeyPressed(Utils::EKey::KEY_LEFT_SHIFT) ? 2.0f : 1.0f);
		Math::Matrix velRotation = Math::Matrix::CreateRotationY(m_CameraAngle);
		m_CameraCenter += velRotation * pAnim->animHandler.getRootNodeVelocity() * m_MainLoopTimer.getAvgDelta().count() * speedMod;

		// Update player lighting
		Physics::RayTestResult r = physicsSystem()->rayTest(m_CameraCenter + Math::float3(0.0f,3.0f,0.0f), m_CameraCenter + Math::float3(0,-2000,0), Physics::CT_WorldMesh);

		if(r.hitTriangleIndex != UINT_MAX)
		{
			pVisual->colorMod = m_TestWorld->getWorldMesh().getTriangleList()[r.hitTriangleIndex].interpolateLighting(r.hitPosition);
		}
	}
	else
	{
		if(m_Window->getKeyPressed(Utils::EKey::KEY_W) || m_Window->getKeyPressed(Utils::EKey::KEY_UP))
			m_CameraCenter += Math::float3(sinf(m_CameraAngle), 0, cosf(m_CameraAngle)) * movement;

		if(m_Window->getKeyPressed(Utils::EKey::KEY_A))
			m_CameraCenter -= Math::float3::cross(Math::float3(0, 1, 0), Math::float3(sinf(m_CameraAngle), 0, cosf(m_CameraAngle))).normalize() * movement;

		if(m_Window->getKeyPressed(Utils::EKey::KEY_S) || m_Window->getKeyPressed(Utils::EKey::KEY_DOWN))
			m_CameraCenter -= Math::float3(sinf(m_CameraAngle), 0, cosf(m_CameraAngle)) * movement;

		if(m_Window->getKeyPressed(Utils::EKey::KEY_D))
			m_CameraCenter += Math::float3::cross(Math::float3(0, 1, 0), Math::float3(sinf(m_CameraAngle), 0, cosf(m_CameraAngle))).normalize() * movement;

		if(m_Window->getKeyPressed(Utils::EKey::KEY_Q))
			m_CameraCenter.y += movement * 0.5f;

		if(m_Window->getKeyPressed(Utils::EKey::KEY_Z))
			m_CameraCenter.y -= movement * 0.5f;

		if(m_Window->getKeyPressed(Utils::EKey::KEY_SPACE))
		{
			Math::float3 d1 = Math::float3(sinf(m_CameraAngle), 0.0f, cosf(m_CameraAngle)).normalize();
			Math::float3 d2 = Math::float3(sinf(m_CameraAngle), 0.4f, cosf(m_CameraAngle)).normalize();
			for(int i = 0; i < 20; ++i)
				m_Factory.test_createPhysicsEntity(m_CameraCenter - d1 * 3.0f, d2 * -15000.0f);
		}

	}

	Math::Matrix view;

	// Try to put camera on the ground if not flying
	if(!m_IsFlying)
	{
		Physics::RayTestResult result = physicsSystem()->rayTest(m_CameraCenter, m_CameraCenter + Math::float3(0, -100, 0));

		if(result.hitPosition != m_CameraCenter)
		{
			m_CameraCenter.y = result.hitPosition.y + 1.7f * 0.5f;
		}

		const float distance = 3.0f;
		const float height = 2.0f;
		const float targetHeight = 0.5f;
		Math::float3 eye = m_CameraCenter - Math::float3(sinf(m_CameraAngle), 0, cosf(m_CameraAngle)) * distance + Math::float3(0.0f, height, 0.0f);
		Math::float3 target = m_CameraCenter + Math::float3(0.0f, targetHeight, 0.0f);

		view = Math::Matrix::CreateLookAt(eye, 
			target, Math::float3(0, 1, 0));

		Entity* e = objectFactory().storage().getEntity(m_TestWorld->getPlayer());
		Components::AnimationController* pAnim = objectFactory().storage().getComponent<Components::AnimationController>(m_TestWorld->getPlayer());

		Math::Matrix playerWorld = Math::Matrix::CreateTranslation(m_CameraCenter) * Math::Matrix::CreateRotationY(m_CameraAngle);
		e->setWorldTransform(playerWorld);

		
		if(m_Window->getKeyPressed(Utils::EKey::KEY_A) || dirPressed == Utils::EKey::KEY_A)
			pAnim->animHandler.PlayAnimation("T_RUNSTRAFEL");
		else if(m_Window->getKeyPressed(Utils::EKey::KEY_D) || dirPressed == Utils::EKey::KEY_D)
			pAnim->animHandler.PlayAnimation("T_RUNSTRAFER");
		else if(m_Window->getKeyPressed(Utils::EKey::KEY_W) || dirPressed == Utils::EKey::KEY_W)
			pAnim->animHandler.PlayAnimation("S_RUNL");
		else if(m_Window->getKeyPressed(Utils::EKey::KEY_S) || dirPressed == Utils::EKey::KEY_S)
			pAnim->animHandler.PlayAnimation("T_JUMPB");
		else 
			pAnim->animHandler.PlayAnimation("S_LGUARD");
	}
	if(m_IsFlying)
	{
		view = Math::Matrix::CreateLookAt(m_CameraCenter, m_CameraCenter + Math::float3(sinf(m_CameraAngle), 0, cosf(m_CameraAngle)), Math::float3(0, 1, 0));
	}
		
	
	//view = Math::Matrix::CreateLookAt(Math::float3(sinf(m_CameraAngle) * 10, 0, cosf(m_CameraAngle) * 10), Math::float3(0,0,0), Math::float3(0, 1, 0));

	RAPI::RInt2 res = RAPI::REngine::RenderingDevice->GetOutputResolution();

#ifdef RND_GL
	Math::Matrix projection = Math::Matrix::CreatePerspectiveGL(Math::DegToRad(85.0f), res.x, res.y, 0.1f, 10000.0f);
#else
	Math::Matrix projection = Math::Matrix::CreatePerspectiveDX(Math::DegToRad(85.0f), res.x, res.y, 0.1f, 10000.0f);
#endif

	

	Math::Matrix model = Math::Matrix::CreateIdentity();
	Math::Matrix viewProj = projection * view;

	m_pCameraBuffer->UpdateData(&viewProj);

	//RAPI::RTools::LineRenderer.AddPointLocator(RAPI::RFloat3(0, 0, 0), 25.0f);
	//RAPI::RTools::LineRenderer.AddPointLocator(RAPI::RFloat3(10, 0, 0), 25.0f);
	//RAPI::RTools::LineRenderer.AddPointLocator(RAPI::RFloat3(0, 0, 0), 25.0f);
	//RAPI::RTools::LineRenderer.AddPointLocator(RAPI::RFloat3(10, 0, 10), 25.0f);

	/*static ZenConvert::zCModelMeshLib lib("HUMANS.MDH", m_VdfsFileIndex);
	static ZenConvert::zCModelAni ani("HUMANS-S_RUNL.MAN", m_VdfsFileIndex);
	static std::vector<Math::Matrix> aniMatricesLocal(lib.getNodes().size());
	static std::vector<Math::Matrix> objToWorld(lib.getNodes().size());

	if(lib.getNodeChecksum() != ani.getModelAniHeader().nodeChecksum)
		LogWarn() << "Different checksums!";

	static int frame = 0;

	static double s_AnimUpdateMod = 0;
	s_AnimUpdateMod += m_MainLoopTimer.getAvgDelta().count();

	if(s_AnimUpdateMod > 1.0f / ani.getModelAniHeader().fpsRate)
	{
		frame = (frame + 1) % ani.getModelAniHeader().numFrames;
		s_AnimUpdateMod = 0;
	}

	for(size_t i = 0; i < lib.getNodes().size(); i++)
	{
		aniMatricesLocal[i] = lib.getNodes()[i].transformLocal;
	}

	Math::float3 aniTrans(0,0,-30);

	for(size_t i = 0; i < ani.getNodeIndexList().size(); i++)
	{
		Math::Matrix trans;

		auto& sample = ani.getAniSamples()[frame * ani.getNodeIndexList().size() + i];

		uint32_t n = ani.getNodeIndexList()[i];
		trans = Math::Matrix::CreateFromQuaternion(sample.rotation);
		trans.Translation(sample.position);

		aniMatricesLocal[n] = trans;
	}

	for(size_t i = 0; i < lib.getNodes().size(); i++)
	{
		//if(i==0)
		//	aniMatricesLocal[i] = Math::Matrix::CreateIdentity();

		if(lib.getNodes()[i].parentValid())
			objToWorld[i] = objToWorld[lib.getNodes()[i].parentIndex] * aniMatricesLocal[i];
		else 
			objToWorld[i] = aniMatricesLocal[i];
	}

	for(size_t i = 0; i < lib.getNodes().size(); i++)
	{
		const ZenConvert::ModelNode& n  = lib.getNodes()[i];

		Math::float3 p2 = (objToWorld[i].Translation() * 0.01f + aniTrans);

		RAPI::RTools::LineRenderer.AddPointLocator(p2.v, 0.04f);

		if(n.parentValid())
		{
			Math::float3 p1 = (objToWorld[n.parentIndex].Translation() * 0.01f + aniTrans);
			

			RAPI::RTools::LineRenderer.AddLine(RAPI::LineVertex(p1.v, { 1.0f,1.0f,1.0f,1.0f }),
				RAPI::LineVertex(p2.v, { 1.0f,1.0f,1.0f,1.0f }));
		}
	}*/
	
	//Math::Matrix viewProj = projection * view;
	RAPI::RTools::LineRenderer.Flush(reinterpret_cast<float*>(&viewProj));

	m_pRenderSystem->renderFrame(m_MainLoopTimer.getAvgDelta().count());

	return retVal;
}

void Engine::GameEngine::init()
{
	Engine::Engine::init();

	RAPI::REngine::InitializeEngine();
	RAPI::REngine::RenderingDevice->CreateDevice();

#ifdef RAPI_USE_GLES3
	Renderer::EGL_Window* window = reinterpret_cast<Renderer::EGL_Window*>(m_Window);
	static bool s_surfaceSet = false;
	if(!s_surfaceSet)
	{
		while(!window->getSurface())
			m_Window->pollEvent([&](Renderer::Window::Event ev){});

		if(window->getSurface())
		{
			RAPI::REngine::RenderingDevice->SetWindow(reinterpret_cast<WindowHandle>(window->getSurface()));
			s_surfaceSet = true;
		}
		else
		{
			m_Window->pollEvent([&](Renderer::Window::Event ev){});
			return;
		}
	}
#endif

#ifdef RND_GL
#ifdef RAPI_USE_GLFW
	Renderer::GLFW_Window* window = reinterpret_cast<Renderer::GLFW_Window*>(m_Window);
	RAPI::REngine::RenderingDevice->SetWindow(reinterpret_cast<WindowHandle>(window->getGLFWwindow()));
#endif

#else
	RAPI::REngine::RenderingDevice->SetWindow(reinterpret_cast<WindowHandle>(m_Window->getNativeHandle()));
#endif

	RAPI::RPixelShader* ps = RAPI::RTools::LoadShaderFromString<RAPI::RPixelShader>(fragment_shader, "simplePS");
	RAPI::RStateMachine& sm = RAPI::REngine::RenderingDevice->GetStateMachine();

	RAPI::RVertexShader* vs = RAPI::RTools::LoadShaderFromString<RAPI::RVertexShader>(vertex_shader, "simpleVS");
	RAPI::RVertexShader* vs_skel = RAPI::RTools::LoadShaderFromString<RAPI::RVertexShader>(vertex_shader_skeletal, "skeletalVS");

	// Clear with a sky-blue color
	RAPI::REngine::RenderingDevice->SetMainClearValues(RAPI::RFloat4(0.0f, 0.53f, 1.0f, 0.0f));

	// Create camera-buffer // TODO: Make class for this
	m_pCameraBuffer = RAPI::REngine::ResourceCache->CreateResource<RAPI::RBuffer>();
	

	Math::Matrix view = Math::Matrix::CreateLookAt(m_CameraCenter, m_CameraCenter + Math::float3(sinf(m_CameraAngle), 0, cosf(m_CameraAngle)), Math::float3(0, 1, 0));

	RAPI::RInt2 res = RAPI::REngine::RenderingDevice->GetOutputResolution();
#ifdef RND_GL
	Math::Matrix projection = Math::Matrix::CreatePerspectiveGL(45.0f, res.x, res.y, 0.1f, 10000.0f);
#else
	Math::Matrix projection = Math::Matrix::CreatePerspectiveDX(45.0f, res.x, res.y, 0.1f, 10000.0f);
#endif

	Math::Matrix viewProj = projection * view;
	
	m_pCameraBuffer->Init(&viewProj, sizeof(Math::Matrix), sizeof(Math::Matrix), RAPI::B_CONSTANTBUFFER, RAPI::U_DYNAMIC, RAPI::CA_WRITE);
	RAPI::REngine::ResourceCache->AddToCache("PerFrameCB", m_pCameraBuffer);

	m_pRenderSystem = new Renderer::RenderSystem(*this);

	m_VdfsFileIndex.loadVDF(BASE_DIR + "vdf/Anims.vdf");
	m_VdfsFileIndex.loadVDF(BASE_DIR + "vdf/Anims_Addon.vdf");

	//ZenConvert::zCModelAni ani("HUMANS-S_RUN.MAN", m_VdfsFileIndex);
	
	m_VdfsFileIndex.loadVDF(BASE_DIR + "vdf/Worlds.vdf");
	m_VdfsFileIndex.loadVDF(BASE_DIR + "vdf/Worlds_Addon.vdf");
	m_VdfsFileIndex.loadVDF(BASE_DIR + "vdf/Textures.vdf");
	m_VdfsFileIndex.loadVDF(BASE_DIR + "vdf/Meshes.vdf");
	m_VdfsFileIndex.loadVDF(BASE_DIR + "vdf/Meshes_Addon.vdf");
	m_VdfsFileIndex.loadVDF(BASE_DIR + "vdf/Textures_Addon.vdf");
	m_VdfsFileIndex.loadVDF(BASE_DIR + "vdf/OpenZE.vdf");
	m_VdfsFileIndex.loadVDF(BASE_DIR + "vdf/Anthera.mod");

	//m_TestWorld = new ZenWorld(*this, "anthera_final1.zen", m_VdfsFileIndex);
#ifndef NEW_WORLD
	m_TestWorld = new ZenWorld(*this, "AddonWorld.zen", m_VdfsFileIndex);
#else
	m_TestWorld = new ZenWorld(*this, "NewWorld.zen", m_VdfsFileIndex);
#endif
}
 