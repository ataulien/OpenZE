#include <RTools.h>
#include <RBuffer.h>
#include <REngine.h>
#include <RResourceCache.h>
#include <RStateMachine.h>
#include <RPixelShader.h>
#include <RVertexShader.h>
#include <RInputLayout.h>

#include "utils/GLFW_window.h"
#include "utils/mathlib.h"
#include "renderer/vertextypes.h"
#include "gameengine.h"
#include "components.h"
#include "components/collision.h"
#include "components/visual.h"
#include "zenconvert/zenParser.h"
#include "zenconvert/vob.h"
#include "zenconvert/zCMesh.h"
#include "vdfs/fileIndex.h"
#include "zenconvert/ztex2dds.h"
#include <RTexture.h>

#define RENDER_MASK (::Engine::C_VISUAL)

#ifdef RND_GL
static const char* vertex_shader =
        "#version 420\n"
        "#extension GL_ARB_enhanced_layouts : enable\n"
        "#extension GL_ARB_explicit_uniform_location : enable\n"
        ""
        "layout (std140, binding = 0) uniform shader_data\n"
        "{ \n "
        "	mat4 V_Transform;\n "
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
		"	gl_Position = V_Transform * vec4(vp, 1.0);\n"
		"	f_nrm = mat3(V_Transform) * vnorm;"
		"	f_uv = vuv;"
		"	f_color = vcolor;"
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
		"  frag_colour = vec4(tx.rgb,1) * f_color;"
        "}";
#else
const char* vertex_shader =
        "cbuffer cb : register(b0)"
        "{"
        "Matrix V_Transform;"
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
        "  output.position = mul(V_Transform, float4(input.vp, 1.0));"
		"  output.normal = mul((float3x3)V_Transform, input.normal);"
		"  output.color = input.color;"
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
		"  return 2 * input.color * float4(tx.rgb, 1.0);"
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

RAPI::RBuffer* loadZENMesh(const std::string& file, float scale, std::vector<Math::float3>& zenVertices, std::vector<uint32_t>& zenIndices)
{
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

	loadVDFTexture("NW_DUNGEON_WALL_01-C.TEX");

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
    m_Window(200, 200, 800, 600, "OpenZE"),
	m_CameraZoom(1.0f),
	m_CameraAngle(0.0f),
	m_TestWorld(nullptr)
{
}

Engine::GameEngine::~GameEngine()
{
	delete m_TestWorld;
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
		m_Window.setWindowTitle("openZE - FPS: " + std::to_string(1.0 / frameDeltaSec) + " (" + std::to_string(frameDeltaMS) + "ms)");

		s_TitleUpdateMod = 0.0;
	}

	// Grab window-events
    // TODO: Do this in an update-function, not the render-one!
    const float MOVEMENT_SPEED = 140.0f;
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
            break;
        }
    });
	
	if(m_Window.getKeyPressed(Utils::EKey::KEY_LEFT))
		m_CameraAngle += turn;
	
	if(m_Window.getKeyPressed(Utils::EKey::KEY_RIGHT))
		m_CameraAngle -= turn;
	
	if(m_Window.getKeyPressed(Utils::EKey::KEY_UP))
		m_CameraZoom -= m_MainLoopTimer.getAvgDelta().count() / ZOOM_SPEED;
	
	if(m_Window.getKeyPressed(Utils::EKey::KEY_DOWN))
		m_CameraZoom += m_MainLoopTimer.getAvgDelta().count() / ZOOM_SPEED;
	
	if(m_Window.getKeyPressed(Utils::EKey::KEY_W))
		m_CameraCenter -= Math::float3(sinf(m_CameraAngle), 0, cosf(m_CameraAngle)) * movement;
	
	if(m_Window.getKeyPressed(Utils::EKey::KEY_A))
		m_CameraCenter -= Math::float3::cross(Math::float3(0,1,0), Math::float3(sinf(m_CameraAngle), 0, cosf(m_CameraAngle))).normalize() * movement;	

	if(m_Window.getKeyPressed(Utils::EKey::KEY_S))
		m_CameraCenter += Math::float3(sinf(m_CameraAngle), 0, cosf(m_CameraAngle)) * movement;
	
	if(m_Window.getKeyPressed(Utils::EKey::KEY_D))
		m_CameraCenter += Math::float3::cross(Math::float3(0,1,0), Math::float3(sinf(m_CameraAngle), 0, cosf(m_CameraAngle))).normalize() * movement;
	
	if(m_Window.getKeyPressed(Utils::EKey::KEY_Q))
		m_CameraCenter.y += movement;
	
	if(m_Window.getKeyPressed(Utils::EKey::KEY_Y) || m_Window.getKeyPressed(Utils::EKey::KEY_Z))
		m_CameraCenter.y -= movement;
	
	if(m_Window.getKeyPressed(Utils::EKey::KEY_SPACE))
	{
		Math::float3 d1 = Math::float3(sinf(m_CameraAngle), 0.0f, cosf(m_CameraAngle)).normalize();
		Math::float3 d2 = Math::float3(sinf(m_CameraAngle), 0.4f, cosf(m_CameraAngle)).normalize();
		m_Factory.test_createPhysicsEntity(m_CameraCenter - d1 * 3.0f, d2 * -15000.0f);
	}

	Math::Matrix view = Math::Matrix::CreateLookAt(m_CameraCenter + Math::float3(sinf(m_CameraAngle),0,cosf(m_CameraAngle)), m_CameraCenter, Math::float3(0,1,0));

	RAPI::RInt2 res = RAPI::REngine::RenderingDevice->GetOutputResolution();

#ifdef RND_GL
	Math::Matrix projection = Math::Matrix::CreatePerspectiveGL(45.0f, res.x, res.y, 0.1f, 10000.0f);
#else
	Math::Matrix projection = Math::Matrix::CreatePerspectiveDX(45.0f, res.x, res.y, 0.1f, 10000.0f);
#endif

	// Draw frame
    RAPI::REngine::RenderingDevice->OnFrameStart();
    RAPI::RRenderQueueID queueID = RAPI::REngine::RenderingDevice->AcquireRenderQueue();

    Math::Matrix model = Math::Matrix::CreateIdentity();

    Utils::Vector<Entity> &entities = m_Factory.storage().getEntities();
    for(uint32_t i = 0, end = entities.size(); i < end; ++i)
    {
        Entity &entity = entities[i];
        if((entity.mask & C_VISUAL) != C_VISUAL)
            continue;

        if((entity.mask & C_COLLISION) == C_COLLISION)
        {
            btTransform trans;
            Components::Collision *pCollision = m_Factory.storage().getComponent<Components::Collision>(entity.handle);
            if(pCollision)
            {
                pCollision->rigidBody.getMotionState()->getWorldTransform(trans);
                trans.getOpenGLMatrix(reinterpret_cast<float *>(&model));
            }
            else
                LogWarn() << "Invalid collision object";
        }

        Math::Matrix viewProj = projection * view * model;
        Components::Visual *pVisual = m_Factory.storage().getComponent<Components::Visual>(entity.handle);
        pVisual->pObjectBuffer->UpdateData(&viewProj);

        RAPI::REngine::RenderingDevice->QueuePipelineState(pVisual->pPipelineState, queueID);

        RAPI::REngine::RenderingDevice->ProcessRenderQueue(queueID);
    }

	//Math::Matrix viewProj = projection * view;
	//RAPI::RTools::LineRenderer.Flush(reinterpret_cast<float*>(&viewProj));

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


	VDFS::FileIndex idx;
	idx.loadVDF("Textures.vdf");
	idx.loadVDF("Textures_Addon.vdf");
	idx.loadVDF("Anthera.mod");

	m_TestWorld = new Renderer::ZenWorld("Anthera_Final1.zen", idx);
}
