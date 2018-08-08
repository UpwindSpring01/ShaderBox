// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <Windows.h>

#define _USE_MATH_DEFINES // for C++  
#include <cmath>

#include <string>
#include <cassert>
#include <memory>
#include <algorithm>
#include <vector>
#include <map>
#include <unordered_map>
#include <stack>
#include <future>

// DirectX
#include <dxgi1_5.h>
#pragma comment(lib, "dxgi.lib")

#include <d3d11_4.h>
#pragma comment(lib, "d3d11.lib")

#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib") 

#include <xapobase.h>
#pragma comment(lib, "XAPOBase.lib")

#include <DirectXMath.h>

#include <DirectXMesh.h>
#include <DirectXMesh.inl>
#pragma comment(lib, "DirectXMesh.lib")

#include "DirectXTex.h"
#pragma comment(lib, "DirectXTex.lib")

#include "ScreenGrab.h"
#pragma comment(lib, "DirectXTK.lib")

#include <Wincodec.h>


using namespace DirectX;

template<class T>
inline void SafeRelease(T& ppT)
{
	if (ppT)
	{
		ppT->Release();
		ppT = nullptr;
	}
}

inline bool XMFloat4Equals(const XMFLOAT4& a, const XMFLOAT4& b)
{
	return (a.x == b.x) && (a.y == b.y) && (a.z == b.z) && (a.w == b.w);
}

#include "../Extern/DirectXMesh/Utilities/WaveFrontReader.h"

#include "TypeID.h"

#include "GameTime.h"
#include "../Helpers/StructHelpers.h"
#include "../Component/TransformComponent.h"
#include "../Content/ContentManager.h"
#include "../SceneGraph/GameScene.h"
#include "../SceneGraph/GameObject.h"
//#include "MaterialManager.h"

#include "../Mesh/Mesh.h"
#include "../Content/MeshLoader.h"
#include "../Content/EffectLoader.h"
#include "../Content/TextureDataLoader.h"
#include "../Graphics/Material.h"
#include "../Component/MeshDrawComponent.h"
#include "SpringWind.h"
#ifdef _DEBUG
#include "../DebugOnly/Logger.h"
#define CHECK_HR(x) { \
	long check_HR_Line = __LINE__; \
	wchar_t check_HR_Func[sizeof(__func__)]; \
	wchar_t check_HR_File[sizeof(__FILE__)]; \
	size_t check_HR_OutSize = 0; \
	mbstowcs_s(&check_HR_OutSize, check_HR_Func, sizeof(__func__), __func__, sizeof(__func__)); \
	check_HR_OutSize = 0; \
	mbstowcs_s(&check_HR_OutSize, check_HR_File, sizeof(__FILE__), __FILE__, sizeof(__FILE__)); \
	Logger::LogHRESULT(x, std::wstring(check_HR_File) + L": " + std::wstring(check_HR_Func) + L" (Line: " + std::to_wstring(check_HR_Line) + L")"); \
}
#else 
#define CHECK_HR(x) x
#endif

