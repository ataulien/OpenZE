#include <iostream>
#include <REngine.h>
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <RTools.h>
#include <RBuffer.h>
#include <REngine.h>
#include <RResourceCache.h>
#include <RStateMachine.h>
#include <RPixelShader.h>
#include <RVertexShader.h>

#include <RInputLayout.h>
#include "logger.h"
#include "utils/mathlib.h"

using namespace std;
using namespace RAPI;

#ifdef RND_GL
const char* vertex_shader =
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

const char* fragment_shader =
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



RPipelineState* g_Triangle = nullptr;
RBuffer* g_ConstantBuffer = nullptr;


/** We pack most of Gothics FVF-formats into this vertex-struct */
struct SimpleVertex
{
	static const RAPI::INPUT_ELEMENT_DESC INPUT_LAYOUT_DESC[1]; 

	float Position[3];

};

const RAPI::INPUT_ELEMENT_DESC SimpleVertex::INPUT_LAYOUT_DESC[1] =
{
	{ "POSITION", 0, RAPI::FORMAT_R32G32B32_FLOAT, 0, 0xFFFFFFFF, RAPI::INPUT_PER_VERTEX_DATA, 0 },
};


void Update()
{
	static float s_t = 0;
	s_t += 0.01f;

	Sleep(1);

	Math::Matrix model = Math::Matrix::CreateRotationY(s_t) * Math::Matrix::CreateTranslation(0, sinf(s_t), 0);
	Math::Matrix view = Math::Matrix::CreateLookAt(Math::float3(4,3,3), Math::float3(0,0,0), Math::float3(0,1,0));

	RInt2 res = REngine::RenderingDevice->GetOutputResolution();

#ifdef RND_GL
	Math::Matrix projection = Math::Matrix::CreatePerspectiveGL(45.0f, res.x, res.y, 0.1f, 100.0f);
#else
	Math::Matrix projection = Math::Matrix::CreatePerspectiveDX(45.0f, res.x, res.y, 0.1f, 100.0f);
#endif
	Math::Matrix viewProj = projection * view * model;
	//Math::Matrix viewProj = model * view * projection;
	//viewProj = viewProj.Transpose();

	//RFloat4 pos = RFloat4(sinf(s_t) * 0.1f, cosf(s_t) * 0.1f, 0, 0);
	g_ConstantBuffer->UpdateData(&viewProj);

	REngine::RenderingDevice->OnFrameStart();

	RRenderQueueID queue = REngine::RenderingDevice->AcquireRenderQueue();
	REngine::RenderingDevice->QueuePipelineState(g_Triangle, queue);

	REngine::RenderingDevice->ProcessRenderQueue(queue);

	REngine::RenderingDevice->OnFrameEnd();

	REngine::RenderingDevice->Present();
}

void RunApp(GLFWwindow* window)
{
	RPixelShader* ps = RTools::LoadShaderFromString<RPixelShader>(fragment_shader, "simplePS");
	RBuffer* b = REngine::ResourceCache->CreateResource<RBuffer>();
	RBuffer* cb = REngine::ResourceCache->CreateResource<RBuffer>();
	RStateMachine& sm = REngine::RenderingDevice->GetStateMachine();

	RVertexShader* vs = RTools::LoadShaderFromString<RVertexShader>(vertex_shader, "simpleVS");
	RInputLayout* inputLayout = RTools::CreateInputLayoutFor<SimpleVertex>(vs);

	SimpleVertex p[6] = { { 0.0f,  0.5f,  0.0f},
	{0.5f, -0.5f,  0.0f},
	{-0.5f, -0.5f,  0.0f},
	{ 0.0f,  0.5f,  0.0f},
	{-0.5f, -0.5f,  0.0f},
	{0.5f, -0.5f,  0.0f},
	 };

	b->Init(p, sizeof(p), sizeof(SimpleVertex));
	cb->Init(nullptr, sizeof(Math::Matrix), sizeof(Math::Matrix), EBindFlags::B_CONSTANTBUFFER, EUsageFlags::U_DYNAMIC, ECPUAccessFlags::CA_WRITE);
	g_ConstantBuffer = cb;

	sm.SetVertexBuffer(0, b);
	sm.SetPixelShader(ps);
	sm.SetVertexShader(vs);
	sm.SetInputLayout(inputLayout);
	sm.SetConstantBuffer(0, cb, EShaderType::ST_VERTEX);

	g_Triangle = sm.MakeDrawCall(6);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		Update();

		/* Poll for and process events */
		glfwPollEvents();	
	}

	REngine::ResourceCache->DeleteResource(b);
}

#ifdef WIN32
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
#else
int main(int argc, char** argv)
#endif
{
	Log::Clear();

	//Window wnd;
	//wnd.Create("--- Test ---", 20, 20, 1280, 720, hInstance, WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	GLFWwindow* wnd = glfwCreateWindow(1280, 720, "--- Test ---", nullptr, nullptr);



	if(!wnd)
	{
		glfwTerminate();
		return -1;
	}



	REngine::InitializeEngine();
	REngine::RenderingDevice->CreateDevice();

#ifdef RND_GL
	REngine::RenderingDevice->SetWindow(wnd);
#else
	REngine::RenderingDevice->SetWindow(glfwGetWin32Window(wnd));
#endif
	cout << "Hello, World!" << endl;

	RunApp(wnd);

	REngine::UninitializeEngine();

	glfwTerminate();

	return 0;
}
