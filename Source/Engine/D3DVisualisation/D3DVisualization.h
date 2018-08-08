#pragma once



extern "C"
{
	__declspec(dllexport) HRESULT __cdecl Init();
}

extern "C"
{
	__declspec(dllexport) HRESULT __cdecl Cleanup();
}

extern "C" 
{
	__declspec(dllexport) HRESULT __cdecl Render(void * pResource, bool isNewSurface);
}

extern "C"
{
	__declspec(dllexport) HRESULT __cdecl RenderThumbnail(const wchar_t* model, const wchar_t* saveLocation);
}

extern "C"
{
	__declspec(dllexport) HRESULT __cdecl RenderThumbnailActive(const wchar_t* saveLocation);
}

extern "C"
{
	__declspec(dllexport) HRESULT __cdecl SetModel(wchar_t* filename);
}

extern "C"
{
	__declspec(dllexport) HRESULT __cdecl SetTopology(const int topology);
}

extern "C"
{
	__declspec(dllexport) HRESULT __cdecl UpdateShaderVariables(const UINT cbufferIndex, const int shaderType, const byte* data, const UINT length);
}

extern "C"
{
	__declspec(dllexport) HRESULT __cdecl SetRasterizerState(const int cullMode, const int fillMode);
}

extern "C"
{
	__declspec(dllexport) HRESULT __cdecl SetCameraOffset(const float offset);
}


extern "C"
{
	__declspec(dllexport) HRESULT __cdecl UpdateShaderImage(const UINT cbufferIndex, const int shaderType, const wchar_t* path);
}

extern "C"
{
	__declspec(dllexport) HRESULT __cdecl UpdateShaders(wchar_t* identifier, wchar_t* vertex, wchar_t* hull, wchar_t* domain, wchar_t* geometry, wchar_t* pixel);
}

extern "C"
{
	__declspec(dllexport) HRESULT __cdecl UpdateShadersPP(wchar_t* identifier, wchar_t* vertex, wchar_t* pixel);
}

extern "C"
{
	__declspec(dllexport) HRESULT __cdecl Reset();
}
