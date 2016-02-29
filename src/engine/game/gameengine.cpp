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
#include "utils/GLFW_window.h"
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

#define RENDER_MASK (::Engine::C_VISUAL)

#ifdef RND_GL
static const char* vertex_shader =
        "#version 420\n"
        "#extension GL_ARB_enhanced_layouts : enable\n"
        "#extension GL_ARB_explicit_uniform_location : enable\n"
        ""
		"layout (std140, binding = 0) uniform buffer0\n"
		"{ \n "
		"	mat4 PF_ViewProj;\n "
		"}; \n "
		"layout (std140, binding = 1) uniform buffer1\n"
		"{ \n "
		"	mat4 V_WorldMatrix;\n "
		"   vec4 V_Color;"
		"}; \n "
        "\n "
        "\n "
		"in vec3 vp;\n"
		"in vec3 vnorm;\n"
		"in vec2 vuv;\n"
		"in vec4 vcolor;\n"
		"out vec3 f_nrm;\n"
		"out vec2 f_uv;\n"
		"out vec4 f_color;\n"
        "void main () {\n"
		"	gl_Position = PF_ViewProj * V_WorldMatrix * vec4(vp, 1.0);\n"
		"	f_nrm = mat3(V_WorldMatrix) * vnorm;"
		"	f_uv = vuv;"
		"	f_color = vcolor * V_Color;"
        "}\n";

static const char* fragment_shader =
        "#version 420\n"
        "out vec4 frag_colour;"
		"uniform sampler2D texture0;"
		"in vec3 f_nrm;\n"
		"in vec2 f_uv;\n"
		"in vec4 f_color;\n"
        "void main () {"
		"  vec4 tx = texture2D(texture0, f_uv.xy);"
		"  if(tx.a < 0.5) discard;"
		"  vec4 color = mix(f_color * 0.8, f_color, 1.5 * min(1.0, max(0.0,dot(normalize(f_nrm), normalize(vec3(0.333,0.666,0.333))))));"
		" frag_colour = tx * color;"
		//"  frag_colour = vec4(tx.rgb,1) * color;"
        "}";
#else
const char* vertex_shader =


		"cbuffer cb : register(b0)"
		"{"
		"Matrix PF_ViewProj;"
		"};"

		"cbuffer cb : register(b1)"
		"{"
		"Matrix V_WorldMatrix;"
		"float4 V_Color;"
		"};"

		"struct VS_INPUT"
		"{"
		"	float3 vp : POSITION;"
		"	float3 normal : NORMAL;"
		"	float2 texCoord : TEXCOORD;"
		"	float4 color : COLOR;"
		"};"
		"struct VS_OUTPUT"
		"{"		
		"	float3 normal : TEXCOORD0;"
		"	float2 texCoord : TEXCOORD1;"
		"	float4 color : TEXCOORD2;"
		"	float4 position : SV_POSITION;"
		"};"
        "VS_OUTPUT VSMain (VS_INPUT input) {"
		"  VS_OUTPUT output = (VS_OUTPUT)0;"	
		"  output.position = mul(mul(PF_ViewProj, V_WorldMatrix), float4(input.vp, 1.0));"
		"  output.normal = mul((float3x3)V_WorldMatrix, input.normal);"
		"  output.color = input.color * V_Color;"
		"  output.texCoord = input.texCoord;"
		"  return output;"
        "}";

const char* fragment_shader =
		"struct VS_OUTPUT"
		"{"
		"	float3 normal : TEXCOORD0;"
		"	float2 texCoord : TEXCOORD1;"
		"	float4 color : TEXCOORD2;"
		"};"
		"Texture2D	TX_Texture0 : register( t0 );"
		"SamplerState SS_Linear : register( s0 );"
        "float4 PSMain (VS_OUTPUT input) : SV_TARGET {"
		"  float4 tx = TX_Texture0.Sample(SS_Linear, frac(input.texCoord));"
		"  clip(tx.a - 0.5f);"
		"  return input.color * float4(tx.rgb, 1.0);"
        "}";
#endif

RAPI::RTexture* loadVDFTexture(const std::string& file)
{
	VDFS::FileIndex idx;

	idx.loadVDF("Textures.vdf");

	std::vector<uint8_t> testData;
	idx.getFileData("CHAPTER5_PAL-C.TEX", testData);

	std::vector<uint8_t> ddsData;
	ZenConvert::convertZTEX2DDS(testData, ddsData);

	RAPI::RTexture* tx = RAPI::REngine::ResourceCache->CreateResource<RAPI::RTexture>();
	tx->CreateTexture(ddsData.data(), ddsData.size(), RAPI::RInt2(0,0), 0, RAPI::TF_FORMAT_UNKNOWN_DXT);
	
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

		vx[i+0].Normal = nrm;
		vx[i+1].Normal = nrm;
		vx[i+2].Normal = nrm;			
    }
    return nullptr;
}

RAPI::RBuffer* MakeBox(float extends)
{
	const int n = 36;
    Renderer::WorldVertex vx[n];
    int i=0;

	vx[i++].Position = Math::float3(-1.0f,-1.0f,-1.0f);
	vx[i++].Position = Math::float3(-1.0f,-1.0f, 1.0f);
	vx[i++].Position = Math::float3(-1.0f, 1.0f, 1.0f);
	vx[i++].Position = Math::float3(1.0f, 1.0f,-1.0f);
	vx[i++].Position = Math::float3(-1.0f,-1.0f,-1.0f);
	vx[i++].Position = Math::float3(-1.0f, 1.0f,-1.0f);
	vx[i++].Position = Math::float3(1.0f,-1.0f, 1.0f);
	vx[i++].Position = Math::float3(-1.0f,-1.0f,-1.0f);
	vx[i++].Position = Math::float3(1.0f,-1.0f,-1.0f);
	vx[i++].Position = Math::float3(1.0f, 1.0f,-1.0f);
	vx[i++].Position = Math::float3(1.0f,-1.0f,-1.0f);
	vx[i++].Position = Math::float3(-1.0f,-1.0f,-1.0f);
	vx[i++].Position = Math::float3(-1.0f,-1.0f,-1.0f);
	vx[i++].Position = Math::float3(-1.0f, 1.0f, 1.0f);
	vx[i++].Position = Math::float3(-1.0f, 1.0f,-1.0f);
	vx[i++].Position = Math::float3(1.0f,-1.0f, 1.0f);
	vx[i++].Position = Math::float3(-1.0f,-1.0f, 1.0f);
	vx[i++].Position = Math::float3(-1.0f,-1.0f,-1.0f);
	vx[i++].Position = Math::float3(-1.0f, 1.0f, 1.0f);
	vx[i++].Position = Math::float3(-1.0f,-1.0f, 1.0f);
	vx[i++].Position = Math::float3(1.0f,-1.0f, 1.0f);
	vx[i++].Position = Math::float3(1.0f, 1.0f, 1.0f);
	vx[i++].Position = Math::float3(1.0f,-1.0f,-1.0f);
	vx[i++].Position = Math::float3(1.0f, 1.0f,-1.0f);
	vx[i++].Position = Math::float3(1.0f,-1.0f,-1.0f);
	vx[i++].Position = Math::float3(1.0f, 1.0f, 1.0f);
	vx[i++].Position = Math::float3(1.0f,-1.0f, 1.0f);
	vx[i++].Position = Math::float3(1.0f, 1.0f, 1.0f);
	vx[i++].Position = Math::float3(1.0f, 1.0f,-1.0f);
	vx[i++].Position = Math::float3(-1.0f, 1.0f,-1.0f);
	vx[i++].Position = Math::float3(1.0f, 1.0f, 1.0f);
	vx[i++].Position = Math::float3(-1.0f, 1.0f,-1.0f);
	vx[i++].Position = Math::float3(-1.0f, 1.0f, 1.0f);
	vx[i++].Position = Math::float3(1.0f, 1.0f, 1.0f);
	vx[i++].Position = Math::float3(-1.0f, 1.0f, 1.0f);
	vx[i++].Position = Math::float3(1.0f,-1.0f, 1.0f );

	// Generate normals
	for(i = 0; i < n; i += 3)
	{
		std::swap(vx[i+2].Position, vx[i+1].Position);

		Math::float3 v0 = vx[i].Position;
		Math::float3 v1 = vx[i + 1].Position;
		Math::float3 v2 = vx[i + 2].Position;

		Math::float3 nrm = Math::float3::cross(v1 - v0, v2 - v0).normalize();

		vx[i+0].Normal = nrm;
		vx[i+1].Normal = nrm;
		vx[i+2].Normal = nrm;
	}

    // Loop through all vertices and apply the extends
    for(i = 0; i < n; i++)
    {
		vx[i].Color = 0xFFFF0000;
		vx[i].TexCoord = Math::float2(0,0);
        vx[i].Position *= extends;
    }

    RAPI::RBuffer* buffer = RAPI::REngine::ResourceCache->CreateResource<RAPI::RBuffer>();
    buffer->Init(&vx[0], sizeof(vx), sizeof(Renderer::WorldVertex));
    return buffer;
}

Engine::GameEngine::GameEngine(int argc, char *argv[]) :
    Engine(argc, argv),
    m_Window(200, 200, 1280, 720, "OpenZE"),
	m_CameraZoom(1.0f),
	m_CameraAngle(0.0f),
	m_CameraCenter(100,0,0),
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

bool Engine::GameEngine::render(float alpha)
{
	(void) alpha;

    bool retVal = true;

	// Update window-title every N seconds
	static double s_TitleUpdateMod = 0;
	const double TITLE_UPDATE_DELAY = 0.1;
	s_TitleUpdateMod += m_MainLoopTimer.getAvgDelta().count();

	if(s_TitleUpdateMod > TITLE_UPDATE_DELAY)
	{
		// Update header
		double frameDeltaSec = m_MainLoopTimer.getAvgDelta().count();
		double frameDeltaMS = m_MainLoopTimer.getAvgDelta().count() * 1000.0f;
        m_Window.setWindowTitle("openZE - FPS: " + std::to_string(1.0 / frameDeltaSec) 
			+ " (" + std::to_string(frameDeltaMS) + "ms) Vobs: " + std::to_string(m_Factory.storage().getEntities().size())
			+ " Camera: " + (m_IsFlying ? "Flying" : "Walking"));

		s_TitleUpdateMod = 0.0;
	}

	// Grab window-events
    // TODO: Do this in an update-function, not the render-one!
    float MOVEMENT_SPEED = 30.0f * (m_Window.getKeyPressed(Utils::EKey::KEY_LEFT_SHIFT) ? 2.0f : 1.0f) * (m_IsFlying ? 1.0f : 0.25f);
    const float ZOOM_SPEED = 40.0f;
	const float TURN_SPEED = Math::DegToRad(70);
	float movement = MOVEMENT_SPEED * m_MainLoopTimer.getAvgDelta().count();
	float turn  = TURN_SPEED * m_MainLoopTimer.getAvgDelta().count();
    m_Window.pollEvent([&](Utils::Window::Event ev)
    {
        switch(ev.EventType)
        {
        case Utils::Window::EEvent::E_Closed:
            retVal = false;
            break;

        case Utils::Window::E_Resized:
			LogInfo() << "Resized window!";
            RAPI::REngine::RenderingDevice->OnResize();
			break;

		case Utils::Window::E_KeyEvent:
			if(ev.KeyboardEvent.action == Utils::Window::EA_Pressed)
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
					m_Window.switchMode(!m_Window.isFullscreen());
					break;
				}
			}
			break;
        }
    });
	
	if(m_Window.getKeyPressed(Utils::EKey::KEY_LEFT))
		m_CameraAngle -= turn;
	
	if(m_Window.getKeyPressed(Utils::EKey::KEY_RIGHT))
		m_CameraAngle += turn;
	
	if(m_Window.getKeyPressed(Utils::EKey::KEY_UP))
		m_CameraZoom -= m_MainLoopTimer.getAvgDelta().count() / ZOOM_SPEED;
	
	if(m_Window.getKeyPressed(Utils::EKey::KEY_DOWN))
		m_CameraZoom += m_MainLoopTimer.getAvgDelta().count() / ZOOM_SPEED;
	
	if(m_Window.getKeyPressed(Utils::EKey::KEY_W))
		m_CameraCenter -= Math::float3(sinf(m_CameraAngle), 0, cosf(m_CameraAngle)) * movement;
	
	if(m_Window.getKeyPressed(Utils::EKey::KEY_A))
		m_CameraCenter += Math::float3::cross(Math::float3(0,1,0), Math::float3(sinf(m_CameraAngle), 0, cosf(m_CameraAngle))).normalize() * movement;	

	if(m_Window.getKeyPressed(Utils::EKey::KEY_S))
		m_CameraCenter += Math::float3(sinf(m_CameraAngle), 0, cosf(m_CameraAngle)) * movement;
	
	if(m_Window.getKeyPressed(Utils::EKey::KEY_D))
		m_CameraCenter -= Math::float3::cross(Math::float3(0,1,0), Math::float3(sinf(m_CameraAngle), 0, cosf(m_CameraAngle))).normalize() * movement;
	
	if(m_Window.getKeyPressed(Utils::EKey::KEY_Q))
        m_CameraCenter.y += movement * 0.5f;
	
    if(m_Window.getKeyPressed(Utils::EKey::KEY_Z))
        m_CameraCenter.y -= movement * 0.5f;
	
	if(m_Window.getKeyPressed(Utils::EKey::KEY_SPACE))
	{
		Math::float3 d1 = Math::float3(sinf(m_CameraAngle), 0.0f, cosf(m_CameraAngle)).normalize();
		Math::float3 d2 = Math::float3(sinf(m_CameraAngle), 0.4f, cosf(m_CameraAngle)).normalize();
        for(int i = 0; i < 20; ++i)
            m_Factory.test_createPhysicsEntity(m_CameraCenter - d1 * 3.0f, d2 * -15000.0f);
	}

	// Try to put camera on the ground if not flying
	if(!m_IsFlying)
	{
		Math::float3 hit = physicsSystem()->rayTest(m_CameraCenter, m_CameraCenter + Math::float3(0,-100,0));

		if(hit != m_CameraCenter)
		{
			m_CameraCenter.y = hit.y + 1.7f;
		}
	}

	Math::Matrix view = Math::Matrix::CreateLookAt(m_CameraCenter, m_CameraCenter + Math::float3(sinf(m_CameraAngle),0,cosf(m_CameraAngle)), Math::float3(0,1,0));

	RAPI::RInt2 res = RAPI::REngine::RenderingDevice->GetOutputResolution();

#ifdef RND_GL
	Math::Matrix projection = Math::Matrix::CreatePerspectiveGL(45.0f, res.x, res.y, 0.1f, 10000.0f);
#else
	Math::Matrix projection = Math::Matrix::CreatePerspectiveDX(45.0f, res.x, res.y, 0.1f, 10000.0f);
#endif

	// Draw frame
    RAPI::REngine::RenderingDevice->OnFrameStart();
    RAPI::RRenderQueueID queueID = RAPI::REngine::RenderingDevice->AcquireRenderQueue(true, "Main Queue");

    Math::Matrix model = Math::Matrix::CreateIdentity();
	Math::Matrix viewProj = projection * view;

	m_pCameraBuffer->UpdateData(&viewProj);

	// Fixme: Don't update visuals sharing one object-buffer more than once. Better than this.
	RAPI::RBuffer* lastObjectBuffer = nullptr;

    Utils::Vector<Entity> &entities = m_Factory.storage().getEntities();
    for(uint32_t i = 0, end = entities.size(); i < end; ++i)
    {
        Entity &entity = entities[i];
        if((entity.mask & C_VISUAL) != C_VISUAL)
            continue;

		Components::Visual *pVisual = m_Factory.storage().getComponent<Components::Visual>(entity.handle);

		Math::Matrix modelViewProj;
        if((entity.mask & C_COLLISION) == C_COLLISION)
        {
            Components::Collision *pCollision = m_Factory.storage().getComponent<Components::Collision>(entity.handle);
            if(pCollision)
            {
                static_cast<Physics::MotionState*>(pCollision->rigidBody.getMotionState())->openGLMatrix(reinterpret_cast<float*>(&model));

				if(pVisual->tmpWorld != model)
				{
					ZenConvert::VobObjectInfo ocb;
					ocb.color = Math::float4(1.0f, 1.0f, 1.0f, 1.0f);
					ocb.worldMatrix = model;
					pVisual->pObjectBuffer->UpdateData(&ocb);

					pVisual->tmpWorld = model;
				}

            }
            else
                LogWarn() << "Invalid collision object";
		}      
    
        RAPI::REngine::RenderingDevice->QueuePipelineState(pVisual->pPipelineState, queueID);
    }

	RAPI::REngine::RenderingDevice->ProcessRenderQueue(queueID);

	RAPI::RTools::LineRenderer.AddPointLocator(RAPI::RFloat3(0,0,0), 50.0f);

	//Math::Matrix viewProj = projection * view;
	RAPI::RTools::LineRenderer.Flush(reinterpret_cast<float*>(&viewProj));


	m_TestWorld->render(projection * view);

	// Process frame
    RAPI::REngine::RenderingDevice->OnFrameEnd();
    RAPI::REngine::RenderingDevice->Present();

    return retVal;
}

void Engine::GameEngine::init()
{
    Engine::Engine::init();

    RAPI::REngine::InitializeEngine();
    RAPI::REngine::RenderingDevice->CreateDevice();

#ifdef RND_GL
    RAPI::REngine::RenderingDevice->SetWindow(reinterpret_cast<WindowHandle>(m_Window.getGLFWwindow()));
#else
    RAPI::REngine::RenderingDevice->SetWindow(reinterpret_cast<WindowHandle>(m_Window.getNativeHandle()));
#endif

    RAPI::RPixelShader* ps = RAPI::RTools::LoadShaderFromString<RAPI::RPixelShader>(fragment_shader, "simplePS");
    RAPI::RStateMachine& sm = RAPI::REngine::RenderingDevice->GetStateMachine();

    RAPI::RVertexShader* vs = RAPI::RTools::LoadShaderFromString<RAPI::RVertexShader>(vertex_shader, "simpleVS");
    RAPI::RInputLayout* inputLayout = RAPI::RTools::CreateInputLayoutFor<Renderer::WorldVertex>(vs);

	// Clear with a sky-blue color
	RAPI::REngine::RenderingDevice->SetMainClearValues(RAPI::RFloat4(0.0f, 0.53f, 1.0f, 0.0f));

	// Create camera-buffer // TODO: Make class for this
	m_pCameraBuffer = RAPI::REngine::ResourceCache->CreateResource<RAPI::RBuffer>();
	m_pCameraBuffer->Init(nullptr, sizeof(Math::Matrix), sizeof(Math::Matrix), RAPI::B_CONSTANTBUFFER, RAPI::U_DYNAMIC, RAPI::CA_WRITE);
	RAPI::REngine::ResourceCache->AddToCache("PerFrameCB", m_pCameraBuffer);

	m_pRenderSystem = new Renderer::RenderSystem(*this);

	//m_VdfsFileIndex.loadVDF("vdf/Worlds.vdf");
	m_VdfsFileIndex.loadVDF("vdf/Worlds_Addon.vdf");
	m_VdfsFileIndex.loadVDF("vdf/Textures.vdf");
	m_VdfsFileIndex.loadVDF("vdf/Meshes.vdf");
	m_VdfsFileIndex.loadVDF("vdf/Meshes_Addon.vdf");
	m_VdfsFileIndex.loadVDF("vdf/Textures_Addon.vdf");
	//m_VdfsFileIndex.loadVDF("vdf/Anthera.mod");

	//m_TestWorld = new ZenWorld(*this, "anthera_final1.zen", idx);
	m_TestWorld = new ZenWorld(*this, "addonworld.zen", m_VdfsFileIndex);
}
