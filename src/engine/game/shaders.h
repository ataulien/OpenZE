#pragma once

#ifdef RND_GL

static const char* vertex_shader_skeletal = 
#ifdef RAPI_USE_GLES3
"#version 300 es"
#else
"#version 420"
#endif
R"END(
#define MAX_NUM_SKELETAL_NODES 96

layout (std140) uniform buffer0
{  
	mat4 PF_ViewProj; 
};  


layout (std140) uniform buffer1
{  
	mat4 PI_World; 
	vec4 PI_NodeTransforms[4 * MAX_NUM_SKELETAL_NODES];
	vec4 PI_Color; 	
};  


layout (location = 0) in vec3 vNormal;		
layout (location = 1) in vec2 vTex1;		
layout (location = 2) in vec4 vDiffuse;		
layout (location = 3) in vec3 vLocalPosition0;
layout (location = 4) in vec3 vLocalPosition1;
layout (location = 5) in vec3 vLocalPosition2;
layout (location = 6) in vec3 vLocalPosition3;
layout (location = 7) in ivec4 iBoneIndices; 
layout (location = 8) in vec4 vWeights;

out vec3 f_nrm;
out vec2 f_uv;
out vec4 f_color;

vec3 MulBone3( vec3 vInputPos, int nMatrix, float fBlendWeight )
{
    vec3 vResult;
    vResult.x = dot( vInputPos, PI_NodeTransforms[4*nMatrix+0].xyz );
    vResult.y = dot( vInputPos, PI_NodeTransforms[4*nMatrix+1].xyz );
    vResult.z = dot( vInputPos, PI_NodeTransforms[4*nMatrix+2].xyz );
    return vResult * fBlendWeight;
}

vec3 MulBone4( vec4 vInputPos, int nMatrix, float fBlendWeight )
{
    vec4 vResult;

	vResult =  PI_NodeTransforms[(4*nMatrix)+0] * vInputPos.x;
	vResult += PI_NodeTransforms[(4*nMatrix)+1] * vInputPos.y;
	vResult += PI_NodeTransforms[(4*nMatrix)+2] * vInputPos.z;
	vResult += PI_NodeTransforms[(4*nMatrix)+3] * vInputPos.w;
    return vResult.xyz * fBlendWeight;
}


void main () {
	vec3 position = vec3(0.0,0.0,0.0);
	position += MulBone4(vec4(vLocalPosition0, 1.0f), iBoneIndices.x, vWeights.x);
	position += MulBone4(vec4(vLocalPosition1, 1.0f), iBoneIndices.y, vWeights.y);
	position += MulBone4(vec4(vLocalPosition2, 1.0f), iBoneIndices.z, vWeights.z);
	position += MulBone4(vec4(vLocalPosition3, 1.0f), iBoneIndices.w, vWeights.w);

	gl_Position = PF_ViewProj * PI_World * vec4(position, 1.0);
	f_nrm = vNormal;
	f_uv = vTex1;
	f_color = vDiffuse * PI_Color;
})END";

static const char* ninst_vertex_shader = 
#ifdef RAPI_USE_GLES3
"#version 300 es"
#else
"#version 420"
#endif
R"END(layout (std140) uniform buffer0
{  
	mat4 PF_ViewProj; 
};  
layout (std140) uniform buffer1
{  
	mat4 PI_World; 
   vec4 PI_Color; 	
};  
 
 
layout (location = 0) in vec3 vp;	   
layout (location = 1) in vec3 vnorm;   
layout (location = 2) in vec2 vuv;	   
layout (location = 3) in vec4 vcolor;  
out vec3 f_nrm;
out vec2 f_uv;
out vec4 f_color;
void main () {
	gl_Position = PF_ViewProj * PI_World * vec4(vp, 1.0);
	f_nrm = vnorm;
	f_uv = vuv;
	f_color = vcolor * PI_Color;
} )END";


static const char* vertex_shader = 
#ifdef RAPI_USE_GLES3
"#version 300 es"
#else
"#version 420"
#endif
R"END(
layout (std140) uniform buffer0
{  
	mat4 PF_ViewProj; 
};  
 
 
layout (location = 0) in vec3 vp;
layout (location = 1) in vec3 vnorm;
layout (location = 2) in vec2 vuv;
layout (location = 3) in vec4 vcolor;
layout (location = 4) in vec4 instanceColor;
layout (location = 5) in mat4 instanceWorld;
out vec3 f_nrm;
out vec2 f_uv;
out vec4 f_color;
void main () {
	gl_Position = PF_ViewProj * instanceWorld * vec4(vp, 1.0);
	f_nrm = mat3(instanceWorld) * vnorm;
	f_uv = vuv;
	f_color = vcolor * instanceColor;
} )END";

static const char* fragment_shader = 
#ifdef RAPI_USE_GLES3
"#version 300 es"
#else
"#version 420"
#endif
R"END(
uniform sampler2D texture0;
in vec3 f_nrm;
in vec2 f_uv;
in vec4 f_color;
out vec4 frag_colour;
void main () {
  vec4 tx = texture(texture0, f_uv);
  if(tx.a < 0.5) discard;
  vec4 color = mix(f_color, f_color * 1.5, min(1.0, max(0.0,dot(normalize(f_nrm), normalize(vec3(0.333,0.666,0.333))))));
 frag_colour = tx * color;
//  frag_colour = vec4(tx.rgb,1) * color;
})END";

static const char* ntex_fragment_shader = 
#ifdef RAPI_USE_GLES3
"#version 300 es"
#else
"#version 420"
#endif
R"END(
out vec4 frag_colour;
uniform sampler2D texture0;
in vec3 f_nrm;
in vec2 f_uv;
in vec4 f_color;
void main () {
  vec4 color = mix(f_color * 0.5, f_color, 1.5 * min(1.0, max(0.0,dot(normalize(f_nrm), normalize(vec3(0.333,0.666,0.333))))));
 frag_colour = color;
//  frag_colour = vec4(tx.rgb,1) * color;
})END";

#else
const char* vertex_shader = R"END(
cbuffer cb : register(b0)
{
Matrix PF_ViewProj;
};

cbuffer cb : register(b1)
{
Matrix V_WorldMatrix;
float4 V_Color;
};

struct VS_INPUT
{
	float3 vp : POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEXCOORD;
	float4 color : COLOR;
	float4 instanceColor : INSTANCE_COLOR;
   Matrix instanceWorld : INSTANCE_WORLD;
};
struct VS_OUTPUT
{
	float3 normal : TEXCOORD0;
	float2 texCoord : TEXCOORD1;
	float4 color : TEXCOORD2;
	float4 position : SV_POSITION;
};
VS_OUTPUT VSMain (VS_INPUT input) {
  VS_OUTPUT output = (VS_OUTPUT)0;
  output.position = mul(mul(PF_ViewProj, input.instanceWorld), float4(input.vp, 1.0));
  output.normal = mul((float3x3)input.instanceWorld, input.normal);
  output.color = input.color * input.instanceColor;
  output.texCoord = input.texCoord;
  return output;
};)END";


const char* vertex_shader_skeletal = R"END(
static const int MAX_NUM_SKELETAL_NODES = 96;
cbuffer cb : register(b0)
{
Matrix PF_ViewProj;
};

cbuffer cb : register(b1)
{
		Matrix PI_WorldMatrix;
		Matrix PI_NodeTransforms[MAX_NUM_SKELETAL_NODES];
		float4 PI_Color;
};

struct VS_INPUT
{
	float3 vNormal		: NORMAL;
	float2 vTex1		: TEXCOORD0;
	float4 vDiffuse		: COLOR;
	float3 vLocalPosition[4]	: LOCALPOSITION;
	uint4  iBoneIndices : BONEIDS;
	float4 vWeights 	: WEIGHTS;
};

struct VS_OUTPUT
{
	float3 normal : TEXCOORD0;
	float2 texCoord : TEXCOORD1;
	float4 color : TEXCOORD2;
	float4 position : SV_POSITION;
};
VS_OUTPUT VSMain (VS_INPUT input) {
  VS_OUTPUT output = (VS_OUTPUT)0;

  float3 position = float3(0,0,0);
  for(int i=0;i<4;i++)
  {
     position += input.vWeights[i] * mul(PI_NodeTransforms[input.iBoneIndices[i]], float4(input.vLocalPosition[i], 1.0f)).xyz;
  }

  output.position = mul(mul(PF_ViewProj, PI_WorldMatrix), float4(position, 1.0));
  output.normal = mul((float3x3)PI_WorldMatrix, input.vNormal);
  output.color = input.vDiffuse * PI_Color;
  output.texCoord = input.vTex1;
  return output;
})END";

const char* fragment_shader = R"END(
struct VS_OUTPUT
{
	float3 normal : TEXCOORD0;
	float2 texCoord : TEXCOORD1;
	float4 color : TEXCOORD2;
};
Texture2D	TX_Texture0 : register( t0 );
SamplerState SS_Linear : register( s0 );
float4 PSMain (VS_OUTPUT input) : SV_TARGET {
  float4 tx = TX_Texture0.Sample(SS_Linear, frac(input.texCoord));
  clip(tx.a - 0.5f);
  return input.color * float4(tx.rgb, 1.0);
})END";
#endif
