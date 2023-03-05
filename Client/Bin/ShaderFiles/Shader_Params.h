#ifndef __SHADER_PARAMS_HLSL__
#define __SHADER_PARAMS_HLSL__

matrix g_WorldMatrix;
matrix g_ViewMatrix;
matrix g_ProjMatrix;

matrix g_ProjMatrixInv;
matrix g_ViewMatrixInv;
matrix g_WorldMatrixInv;

vector g_vCamPosition;
// matrix g_matWV;
// matrix g_matWVP;
// matrix g_matViewInv;

float   g_Time;
float   g_Far = 1000.f;

int     g_int_0;
int     g_int_1;
int     g_int_2;
int     g_int_3;
float   g_float_0;
float   g_float_1;
float   g_float_2;
float   g_float_3;
float   g_float_4;
float   g_float_5;
float   g_float_6;
float   g_float_7;
float   g_float_8;
float   g_float_9;
float2  g_vec2_0;
float2  g_vec2_1;
float2  g_vec2_2;
float2  g_vec2_3;
float4  g_vec4_0;
float4  g_vec4_1;
float4  g_vec4_2;
float4  g_vec4_3;
float4x4 g_mat_0;
float4x4 g_mat_1;
// row_major float4x4 g_mat_2;
// row_major float4x4 g_mat_3;

int     g_tex_on_0;
int     g_tex_on_1;
int     g_tex_on_2;
int     g_tex_on_3;
int     g_tex_on_4;
int     g_tex_on_5;
int     g_tex_on_6;
int     g_tex_on_7;
int     g_tex_on_8;
int     g_tex_on_9;
Texture2D g_tex_0;
Texture2D g_tex_1;
Texture2D g_tex_2;
Texture2D g_tex_3;
Texture2D g_tex_4;
Texture2D g_tex_5;
Texture2D g_tex_6;
Texture2D g_tex_7;
Texture2D g_tex_8;
Texture2D g_tex_9;

#endif
