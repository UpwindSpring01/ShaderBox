#include "stdafx.h"

#include "D3DVisualization.h"

BOOL WINAPI DllMain(HINSTANCE /*hInstance*/, DWORD /*fwdReason*/, LPVOID /*lpvReserved*/)
{
	return TRUE;
}

SpringWind* pApplication = nullptr;
MeshDrawComponent* g_pMeshDrawComp = nullptr;
Effect* g_ActiveEffect = nullptr;
Material* g_Material = nullptr;

extern HRESULT __cdecl Init()
{
	pApplication = new SpringWind();
	pApplication->Initialize();
	
	GameObject* obj = new GameObject();
	g_pMeshDrawComp = new MeshDrawComponent();
	
	Mesh* meshPtr = ContentManager::Load<Mesh>({ L"./Resources/.Internal/Models/cube.obj" });
	g_pMeshDrawComp->SetMesh(meshPtr);

	g_ActiveEffect = ContentManager::Load<Effect>({ L".Internal", L"./Resources/.Internal/CSO/vs.cso", L"", L"", L"", L"./Resources/.Internal/CSO/ps.cso" });
	g_ActiveEffect->m_Topology = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	
	g_Material = new Material();
	g_Material->SetEffect(g_ActiveEffect);
	
	meshPtr->SetEffect(g_ActiveEffect);

	g_pMeshDrawComp->SetMaterial(g_Material);

	obj->AddComponent(g_pMeshDrawComp);
	pApplication->GetScene()->AddChild(obj);

	return S_OK;
}

extern HRESULT __cdecl Cleanup()
{
	delete pApplication;
	pApplication = nullptr;

	// MaterialManager is not yet included
	delete g_Material;

	return S_OK;
}

/// <summary>
/// Render for global class instance
/// </summary>
extern HRESULT __cdecl Render(void * pResource, bool isNewSurface)
{
	if (nullptr == pApplication)
	{
		return E_FAIL;
	}

	return pApplication->Render(pResource, isNewSurface);
}

extern HRESULT __cdecl RenderThumbnail(const wchar_t* model, const wchar_t* saveLocation)
{
	return pApplication->RenderTumbnail(std::wstring(model), std::wstring(saveLocation));
}

extern HRESULT __cdecl RenderThumbnailActive(const wchar_t* saveLocation)
{
	return pApplication->RenderTumbnail(std::wstring(saveLocation));
}

extern HRESULT __cdecl Reset()
{
	pApplication->GetScene()->ClearPPShader();
	g_ActiveEffect = ContentManager::Load<Effect>({ L".Internal", L"./Resources/.Internal/CSO/vs.cso", L"", L"", L"", L"./Resources/.Internal/CSO/ps.cso" });
	g_ActiveEffect->m_Topology = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	g_pMeshDrawComp->GetMaterial()->SetEffect(g_ActiveEffect);
	g_pMeshDrawComp->GetMesh()->SetEffect(g_ActiveEffect);

	return S_OK;
}

extern HRESULT __cdecl SetModel(wchar_t* filename)
{
	Mesh* pMesh = ContentManager::Load<Mesh>({ std::wstring(filename) });
	g_pMeshDrawComp->SetMesh(pMesh);
	pMesh->SetEffect(g_ActiveEffect);
	return S_OK;
}

extern HRESULT __cdecl SetTopology(const int topology)
{
	D3D11_PRIMITIVE_TOPOLOGY newTop = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
	switch (topology)
	{
	case 0:
		newTop = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		break;
	case 1:
		newTop = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ;
		break;
	case 2:
		newTop = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
		break;
	}
	if (newTop != g_ActiveEffect->m_Topology)
	{
		g_ActiveEffect->m_Topology = newTop;
		g_pMeshDrawComp->GetMesh()->UpdateBuffers();
	}
	return S_OK;
	
}

extern HRESULT __cdecl UpdateShadersPP(wchar_t* identifier, wchar_t* vertex, wchar_t* pixel)
{
	ContentManager::Update<Effect>({ std::wstring(identifier), std::wstring(vertex), L"", L"", L"", std::wstring(pixel) });

	Effect* pEffect = ContentManager::Load<Effect>({ std::wstring(identifier), std::wstring(vertex), L"", L"", L"", std::wstring(pixel) });
	pApplication->GetScene()->AddPPShader(pEffect);

	g_ActiveEffect = ContentManager::Load<Effect>({ L".Internal", L"./Resources/.Internal/CSO/vs.cso", L"", L"", L"", L"./Resources/.Internal/CSO/ps.cso" });
	g_ActiveEffect->m_Topology = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	g_pMeshDrawComp->GetMaterial()->SetEffect(g_ActiveEffect);
	g_pMeshDrawComp->GetMesh()->SetEffect(g_ActiveEffect);

	return S_OK;
}

extern HRESULT __cdecl UpdateShaders(wchar_t* identifier, wchar_t* vertex, wchar_t* hull, wchar_t* domain, wchar_t* geometry, wchar_t* pixel)
{
	if (pApplication == nullptr)
	{
		return S_OK;
	}
	pApplication->GetScene()->ClearPPShader();
	// Quick fix
	ContentManager::Update<Effect>({ std::wstring(identifier), std::wstring(vertex), std::wstring(hull), std::wstring(domain), std::wstring(geometry), std::wstring(pixel) });

	D3D11_PRIMITIVE_TOPOLOGY top = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	if(g_ActiveEffect != nullptr)
	{
		top = g_ActiveEffect->m_Topology;
	}
	
	g_ActiveEffect = ContentManager::Load<Effect>({ std::wstring(identifier), std::wstring(vertex), std::wstring(hull), std::wstring(domain), std::wstring(geometry), std::wstring(pixel) });
	g_ActiveEffect->m_Topology = top;
	g_pMeshDrawComp->GetMaterial()->SetEffect(g_ActiveEffect);
	g_pMeshDrawComp->GetMesh()->SetEffect(g_ActiveEffect);
	return S_OK;
}

extern HRESULT __cdecl UpdateShaderImage(const UINT cbufferIndex, const int shaderType, const wchar_t* path)
{
	if (pApplication == nullptr)
	{
		return S_OK;
	}
	g_pMeshDrawComp->GetMaterial()->UpdateImageBuffers(cbufferIndex, shaderType, std::wstring(path));

	return S_OK;
}

extern HRESULT __cdecl UpdateShaderVariables(const UINT cbufferIndex, const int shaderType, const byte* data, const UINT length)
{
	if (pApplication == nullptr)
	{
		return S_OK;
	}
	g_pMeshDrawComp->GetMaterial()->UpdateBuffers(cbufferIndex, shaderType, data, length);

	return S_OK;
}

extern HRESULT __cdecl SetRasterizerState(const int cullMode, const int fillMode)
{
	return pApplication->SetRasterizerState(cullMode, fillMode);
}

extern HRESULT __cdecl SetCameraOffset(const float offset)
{
	pApplication->SetCameraOffset(offset);
	return S_OK;
}