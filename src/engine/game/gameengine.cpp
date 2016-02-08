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

#define RENDER_MASK (::Engine::C_COLLISION | ::Engine::C_VISUAL)

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
        "void main () {\n"
        "	gl_Position = V_Transform * vec4(vp, 1.0);\n"
        "}\n";

static const char* fragment_shader =
        "#version 420\n"
        "out vec4 frag_colour;"
        "void main () {"
        "  frag_colour = vec4 (0.5, 0.0, 0.5, 1.0);"
        "}";
#else
const char* vertex_shader =
        "cbuffer cb : register(b0)"
        "{"
        "Matrix V_Transform;"
        "};"
        "float4 VSMain (float3 vp : POSITION) : SV_POSITION {"
        "  return mul(V_Transform, float4(vp, 1.0));"
        "}";

const char* fragment_shader =
        "float4 PSMain () : SV_TARGET {"
        "  return float4 (0.5, 0.5, 0.0, 1.0);"
        "}";
#endif

RAPI::RBuffer* MakeBox(float extends)
{
    Renderer::SimpleVertex vx[36];
    int i=0;

    vx[i++].Position = Math::float3(-1.0f,-1.0f,-1.0f);
    vx[i++].Position = Math::float3(1.0f,-1.0f,1.0f);
    vx[i++].Position = Math::float3(-1.0f,-1.0f,1.0f);
    vx[i++].Position = Math::float3(-1.0f,-1.0f,-1.0f);
    vx[i++].Position = Math::float3(1.0f,-1.0f,-1.0f);
    vx[i++].Position = Math::float3(1.0f,-1.0f,1.0f);
    vx[i++].Position = Math::float3(-1.0f,1.0f,-1.0f);
    vx[i++].Position = Math::float3(-1.0f,1.0f,1.0f);
    vx[i++].Position = Math::float3(1.0f,1.0f,1.0f);
    vx[i++].Position = Math::float3(-1.0f,1.0f,-1.0f);
    vx[i++].Position = Math::float3(1.0f,1.0f,1.0f);
    vx[i++].Position = Math::float3(1.0f,1.0f,-1.0f);
    vx[i++].Position = Math::float3(-1.0f,-1.0f,-1.0f);
    vx[i++].Position = Math::float3(-1.0f,-1.0f,1.0f);
    vx[i++].Position = Math::float3(-1.0f,1.0f,1.0f);
    vx[i++].Position = Math::float3(-1.0f,-1.0f,-1.0f);
    vx[i++].Position = Math::float3(-1.0f,1.0f,1.0f);
    vx[i++].Position = Math::float3(-1.0f,1.0f,-1.0f);
    vx[i++].Position = Math::float3(1.0f,-1.0f,-1.0f);
    vx[i++].Position = Math::float3(1.0f,1.0f,1.0f);
    vx[i++].Position = Math::float3(1.0f,-1.0f,1.0f);
    vx[i++].Position = Math::float3(1.0f,-1.0f,-1.0f);
    vx[i++].Position = Math::float3(1.0f,1.0f,-1.0f);
    vx[i++].Position = Math::float3(1.0f,1.0f,1.0f);
    vx[i++].Position = Math::float3(-1.0f,-1.0f,-1.0f);
    vx[i++].Position = Math::float3(1.0f,1.0f,-1.0f);
    vx[i++].Position = Math::float3(1.0f,-1.0f,-1.0f);
    vx[i++].Position = Math::float3(-1.0f,-1.0f,-1.0f);
    vx[i++].Position = Math::float3(-1.0f,1.0f,-1.0f);
    vx[i++].Position = Math::float3(1.0f,1.0f,-1.0f);
    vx[i++].Position = Math::float3(-1.0f,-1.0f,1.0f);
    vx[i++].Position = Math::float3(1.0f,-1.0f,1.0f);
    vx[i++].Position = Math::float3(1.0f,1.0f,1.0f);
    vx[i++].Position = Math::float3(-1.0f,-1.0f,1.0f);
    vx[i++].Position = Math::float3(1.0f,1.0f,1.0f);
    vx[i++].Position = Math::float3(-1.0f,1.0f,1.0f);

    // Loop through all vertices and apply the extends
    for(i = 0; i < 36; i++)
    {
        vx[i].Position *= extends;
    }

    RAPI::RBuffer* buffer = RAPI::REngine::ResourceCache->CreateResource<RAPI::RBuffer>();
    buffer->Init(&vx[0], sizeof(vx), sizeof(Renderer::SimpleVertex));
    return buffer;
}

Engine::GameEngine::GameEngine(int argc, char *argv[]) :
    Engine(argc, argv),
    m_Window(200, 200, 800, 600, "OpenZE")
{
}

Engine::GameEngine::~GameEngine()
{
    RAPI::REngine::UninitializeEngine();
}

bool Engine::GameEngine::render(float alpha)
{
    bool retVal = true;

    (void) alpha;
    m_Window.pollEvent([&](Utils::Window::EEvent ev)
    {
        switch(ev)
        {
        case Utils::Window::EEvent::E_Closed:
            retVal = false;
            break;

        case Utils::Window::E_Resized:
            std::cout << "Resized window!";
            break;
        }
    });

    RAPI::REngine::RenderingDevice->OnFrameStart();
    RAPI::RRenderQueueID queueID = RAPI::REngine::RenderingDevice->AcquireRenderQueue();
    for(int i = 0; i < m_Factory.getMasks().size(); ++i)
    {
        if((m_Factory.getMasks().m_Data[i] & RENDER_MASK) == RENDER_MASK)
        {
            btTransform trans;
            m_Factory.getCollision(i).pMotionState->getWorldTransform(trans);

            Math::Matrix model;
            trans.getOpenGLMatrix(reinterpret_cast<float *>(&model));
            Math::Matrix view = Math::Matrix::CreateLookAt(Math::float3(50,100,40), Math::float3(0,0,0), Math::float3(0,1,0));

            RAPI::RInt2 res = RAPI::REngine::RenderingDevice->GetOutputResolution();

        #ifdef RND_GL
            Math::Matrix projection = Math::Matrix::CreatePerspectiveGL(45.0f, res.x, res.y, 0.1f, 1000.0f);
        #else
            Math::Matrix projection = Math::Matrix::CreatePerspectiveDX(45.0f, res.x, res.y, 0.1f, 100.0f);
        #endif
            Math::Matrix viewProj = projection * view * model;
            Components::Visual &visual = m_Factory.getVisual(i);
            visual.pObjectBuffer->UpdateData(&viewProj);

            RAPI::REngine::RenderingDevice->QueuePipelineState(visual.pPipelineState, queueID);
        }
    }
    RAPI::REngine::RenderingDevice->ProcessRenderQueue(queueID);
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
    RAPI::REngine::RenderingDevice->SetWindow(reinterpret_cast<WindowHandle>(m_Window.getNativeHandle()));
#else
    RAPI::REngine::RenderingDevice->SetWindow(glfwGetWin32Window(wnd));
#endif
    RAPI::RPixelShader* ps = RAPI::RTools::LoadShaderFromString<RAPI::RPixelShader>(fragment_shader, "simplePS");
    RAPI::RStateMachine& sm = RAPI::REngine::RenderingDevice->GetStateMachine();

    RAPI::RVertexShader* vs = RAPI::RTools::LoadShaderFromString<RAPI::RVertexShader>(vertex_shader, "simpleVS");
    RAPI::RInputLayout* inputLayout = RAPI::RTools::CreateInputLayoutFor<Renderer::SimpleVertex>(vs);
}
