#pragma once

#pragma warning (disable : 4005)
#pragma warning (disable : 4251)
#pragma warning (disable : 4503) // json warn

#include <d3d11.h>
#include <Effects11/d3dx11effect.h>
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>
#include <DirectXTK/ScreenGrab.h>
#include <DirectXTK/SimpleMath.h>
#include <DirectXTK/PrimitiveBatch.h>
#include <DirectXTK/VertexTypes.h>
#include <DirectXTK/Effects.h>
#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>

#include <d3dcompiler.h>
#include <TypeInfo.h>
#include <DirectXCollision.h>

// #define USE_ASSIMPT_CONVERTER

#ifdef USE_ASSIMPT_CONVERTER
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#ifdef _DEBUG
#pragma comment(lib, "assimp-vc140-mtd.lib")
#else
#pragma comment(lib, "assimp-vc140-mt.lib")
#endif
#endif

#include <nlohmann/json_fwd.hpp>
using Json = nlohmann::json;

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <DirectXMath.h>
using namespace DirectX;
using namespace SimpleMath;

#include <cassert>

#include <vector>
#include <list>
#include <queue>
#include <map>
#include <unordered_map>
#include <array>
#include <set>
#include <algorithm>
#include <functional>
#include <atomic>
#include <mutex>
using namespace std;

#include "fmod.h"
#include "fmod.hpp"
#pragma comment(lib, "fmod_vc.lib")

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"

#include "Engine_Macro.h"
#include "Engine_Typedef.h"
#include "Engine_Function.h"
#include "Engine_Struct.h"
#include "RefGuard.h"
#include "Imgui_AppLog.h"

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG

using namespace Engine;