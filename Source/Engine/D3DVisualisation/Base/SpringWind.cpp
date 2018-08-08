#include "stdafx.h"
#include "SpringWind.h"

#include "../Content/ContentManager.h"
#include "../Content/EffectLoader.h"
#include "../Helpers/EffectHelper.h"
#include "../Graphics/SpriteRenderer.h"

#include <fstream>
#include <iostream>

#include <dwmapi.h>
#pragma comment(lib, "Dwmapi.lib")
//extern "C"
//{
//	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
//}
//
//extern "C"
//{
//	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
//}

SpringWind* SpringWind::m_pInstance = nullptr;

SpringWind::SpringWind()
{
#ifdef _DEBUG
	if (m_pInstance != nullptr)
	{
		Logger::LogError(L"Can not instantiate more then one engine.");
	}
#endif // _DEBUG

	m_pInstance = this;
}


SpringWind::~SpringWind()
{
	ContentManager::Release();
	delete m_GameContext.pGameTime;
	delete m_GameContext.pCameraData;

	m_GameContext.pDeviceContext->Release();
	m_GameContext.pDevice->Release();
	delete m_pScene;
	
	SpriteRenderer::DestroyInstance();

	m_pInstance = nullptr;
}

HRESULT SpringWind::Initialize()
{
	HRESULT hr = S_OK;

	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

	// Likely won't be very performant in reference
	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	// DX10 or 11 devices are suitable
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; ++driverTypeIndex)
	{
		hr = D3D11CreateDevice(NULL, driverTypes[driverTypeIndex], NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &m_GameContext.pDevice, &m_featureLevel, &m_GameContext.pDeviceContext);

		if (SUCCEEDED(hr))
		{
			break;
		}
	}

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Could not create a Direct3D 10 or 11 device.", L"Error", MB_ICONHAND | MB_OK);
		return hr;
	}

	////////////////////////
	// INITIALIZE GAME
	////////////////////////
	ContentManager::AddLoader(new EffectLoader());
	ContentManager::AddLoader(new MeshLoader());
	ContentManager::AddLoader(new TextureDataLoader());

	m_GameContext.pGameTime = new GameTime();
	m_GameContext.pGameTime->Reset();
	m_GameContext.pGameTime->Start();
	m_pScene = new GameScene(m_GameContext);
	SpriteRenderer::CreateInstance();

	m_GameContext.pCameraData = new CameraData();

	return hr;
}


HRESULT SpringWind::DestroyBuffers()
{
	SafeRelease(m_MainRT.m_pDepthStencil);
	SafeRelease(m_MainRT.m_pDepthStencilView);

	return S_OK;
}

HRESULT SpringWind::CreateBuffers()
{
	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth = { 0 };
	descDepth.Width = m_Width;
	descDepth.Height = m_Height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	CHECK_HR(m_GameContext.pDevice->CreateTexture2D(&descDepth, NULL, &m_MainRT.m_pDepthStencil));

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	CHECK_HR(m_GameContext.pDevice->CreateDepthStencilView(m_MainRT.m_pDepthStencil, &descDSV, &m_MainRT.m_pDepthStencilView));

	return S_OK;
}

void SpringWind::SetUpViewport()
{
	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (float)m_Width;
	vp.Height = (float)m_Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_GameContext.pDeviceContext->RSSetViewports(1, &vp);
}

HRESULT SpringWind::InitRenderTarget(void* pResource)
{
	HRESULT hr = S_OK;

	IUnknown* pUnk = (IUnknown*)pResource;

	IDXGIResource* pDXGIResource;
	CHECK_HR(pUnk->QueryInterface(__uuidof(IDXGIResource), (void**)&pDXGIResource));

	HANDLE sharedHandle;
	CHECK_HR(pDXGIResource->GetSharedHandle(&sharedHandle));

	pDXGIResource->Release();

	IUnknown* tempResource11;
	CHECK_HR(m_GameContext.pDevice->OpenSharedResource(sharedHandle, __uuidof(ID3D11Resource), (void**)(&tempResource11)));

	ID3D11Texture2D* pOutputResource;
	CHECK_HR(tempResource11->QueryInterface(__uuidof(ID3D11Texture2D), (void**)(&pOutputResource)));
	tempResource11->Release();

	D3D11_RENDER_TARGET_VIEW_DESC rtDesc;
	rtDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtDesc.Texture2D.MipSlice = 0;

	CHECK_HR(m_GameContext.pDevice->CreateRenderTargetView(pOutputResource, &rtDesc, &m_MainRT.m_pRenderTargetView));
	CHECK_HR(m_GameContext.pDevice->CreateShaderResourceView(pOutputResource, nullptr, &m_MainRT.m_pShaderResourceView));

	D3D11_TEXTURE2D_DESC outputResourceDesc;
	pOutputResource->GetDesc(&outputResourceDesc);
	if (outputResourceDesc.Width != m_Width || outputResourceDesc.Height != m_Height)
	{
		m_Width = outputResourceDesc.Width;
		m_Height = outputResourceDesc.Height;

		DestroyBuffers();
		CreateBuffers();
		SetUpViewport();
	}

	m_GameContext.pDeviceContext->OMSetRenderTargets(1, &m_MainRT.m_pRenderTargetView, m_MainRT.m_pDepthStencilView);

	if (pOutputResource != nullptr)
	{
		pOutputResource->Release();
	}

	return hr;
}

HRESULT SpringWind::InitThumbnailRenderTarget()
{
	if (m_ThumbRT.m_pRenderTargetView == nullptr)
	{
		//RESOURCE
		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));

		textureDesc.Width = THUMB_SIZE;
		textureDesc.Height = THUMB_SIZE;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;


		CHECK_HR(m_GameContext.pDevice->CreateTexture2D(&textureDesc, nullptr, &m_ThumbRT.m_pColor));

		CHECK_HR(m_GameContext.pDevice->CreateRenderTargetView(m_ThumbRT.m_pColor, nullptr, &m_ThumbRT.m_pRenderTargetView));

		CHECK_HR(m_GameContext.pDevice->CreateShaderResourceView(m_ThumbRT.m_pColor, nullptr, &m_ThumbRT.m_pShaderResourceView));

		// Create depth stencil texture
		D3D11_TEXTURE2D_DESC descDepth = { 0 };
		descDepth.Width = THUMB_SIZE;
		descDepth.Height = THUMB_SIZE;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;
		CHECK_HR(m_GameContext.pDevice->CreateTexture2D(&descDepth, NULL, &m_ThumbRT.m_pDepthStencil));

		// Create the depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(descDSV));
		descDSV.Format = descDepth.Format;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		CHECK_HR(m_GameContext.pDevice->CreateDepthStencilView(m_ThumbRT.m_pDepthStencil, &descDSV, &m_ThumbRT.m_pDepthStencilView));

		UpdatePPTargets(m_ThumbPPRT, THUMB_SIZE, THUMB_SIZE);
	}

	m_GameContext.pDeviceContext->OMSetRenderTargets(1, &m_ThumbRT.m_pRenderTargetView, m_ThumbRT.m_pDepthStencilView);

	D3D11_VIEWPORT vp;
	vp.Width = (float)THUMB_SIZE;
	vp.Height = (float)THUMB_SIZE;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_GameContext.pDeviceContext->RSSetViewports(1, &vp);

	static float ClearColor[4] = { 0.0f, 0.2f, 0.2f, 1.0f };
	m_GameContext.pDeviceContext->ClearRenderTargetView(m_ThumbRT.m_pRenderTargetView, ClearColor);
	m_GameContext.pDeviceContext->ClearDepthStencilView(m_ThumbRT.m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	return S_OK;
}

void SpringWind::SetCameraAspectRatio(float aspectRatio)
{
	m_GameContext.pCameraData->m_Projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspectRatio, 0.01f, 100.0f);

	XMVECTOR worldPosition = XMVectorSet(0, 5, -5, 0);
	XMVECTOR lookAt = XMVectorSet(0, 0, 0, 0);
	XMVECTOR upVec = XMVectorSet(0, 0, 1, 0);

	m_GameContext.pCameraData->m_View = XMMatrixLookAtLH(worldPosition, lookAt, upVec);
	m_GameContext.pCameraData->m_ViewInv = XMMatrixInverse(nullptr, m_GameContext.pCameraData->m_View);

	SetCameraOffset(m_CameraOffset);
}

void SpringWind::SetCameraOffset(float offset)
{
	XMVECTOR worldPosition = XMVectorSet(0, 5, -5, 0);
	XMVECTOR upVec = XMVectorSet(0, 0, 1, 0);
	XMVECTOR lookAt = XMVectorSet(0, 0, 0, 0);
	XMVECTOR result = XMVectorSubtract(lookAt, worldPosition);
	result = XMVector3Normalize(result);

	worldPosition = worldPosition - result * offset;
	
	if (m_GameContext.pCameraData != nullptr)
	{
		m_GameContext.pCameraData->m_View = XMMatrixLookAtLH(worldPosition, lookAt, upVec);
		m_GameContext.pCameraData->m_ViewInv = XMMatrixInverse(nullptr, m_GameContext.pCameraData->m_View);
	}
	m_CameraOffset = offset;

}

HRESULT SpringWind::RenderTumbnail(const std::wstring model, const std::wstring saveLocation)
{
	ID3D11RasterizerState* oldRSState;
	m_GameContext.pDeviceContext->RSGetState(&oldRSState);

	SetRasterizerState(0, 0);

	m_IsRenderingThumb = true;
	GameScene* defaultScene = m_pScene;
	CameraData* defaultCamera = m_GameContext.pCameraData;

	InitThumbnailRenderTarget();
	m_GameContext.pCameraData = new CameraData();
	SetCameraAspectRatio(1.0f);

	GameObject* obj = new GameObject();
	MeshDrawComponent* pMeshDrawComp = new MeshDrawComponent();

	Mesh* meshPtr = ContentManager::Load<Mesh>({ model });
	pMeshDrawComp->SetMesh(meshPtr);

	Effect* effectPtr = ContentManager::Load<Effect>({ L".Internal", L"./Resources/.Internal/vs.cso", L"", L"", L"", L"./Resources/.Internal/ps.cso" });
	effectPtr->m_Topology = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	Material* materialPtr = new Material();
	materialPtr->SetEffect(effectPtr);

	meshPtr->SetEffect(effectPtr);

	pMeshDrawComp->SetMaterial(materialPtr);

	obj->AddComponent(pMeshDrawComp);

	m_pScene = new GameScene(m_GameContext);
	m_pScene->AddChild(obj);

	m_pScene->Draw(m_GameContext);
	CHECK_HR(SaveWICTextureToFile(m_GameContext.pDeviceContext, m_ThumbRT.m_pColor, GUID_ContainerFormatPng, saveLocation.c_str()));

	delete m_pScene;
	delete m_GameContext.pCameraData;
	delete materialPtr;

	// Reset
	m_IsRenderingThumb = false;
	m_pScene = defaultScene;
	m_pScene->BindBuffers();
	m_GameContext.pCameraData = defaultCamera;
	m_GameContext.pDeviceContext->OMSetRenderTargets(1, &m_MainRT.m_pRenderTargetView, m_MainRT.m_pDepthStencilView);
	SetUpViewport();

	m_GameContext.pDeviceContext->RSSetState(oldRSState);
	if (oldRSState)
	{
		oldRSState->Release();
	}
	return S_OK;
}

HRESULT SpringWind::RenderTumbnail(const std::wstring saveLocation)
{
	m_IsRenderingThumb = true;
	CameraData* defaultCamera = m_GameContext.pCameraData;

	InitThumbnailRenderTarget();
	m_GameContext.pCameraData = new CameraData();
	SetCameraAspectRatio(1.0f);
	m_pScene->Draw(m_GameContext);
	CHECK_HR(SaveWICTextureToFile(m_GameContext.pDeviceContext, m_ThumbRT.m_pColor, GUID_ContainerFormatPng, saveLocation.c_str()));

	delete m_GameContext.pCameraData;

	// Reset
	m_IsRenderingThumb = false;
	m_GameContext.pCameraData = defaultCamera;
	m_GameContext.pDeviceContext->OMSetRenderTargets(1, &m_MainRT.m_pRenderTargetView, m_MainRT.m_pDepthStencilView);
	SetUpViewport();

	return S_OK;
}

HRESULT SpringWind::SetRasterizerState(const int cullMode, const int fillMode)
{
	D3D11_RASTERIZER_DESC desc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
	switch (cullMode)
	{
	case 0:
		desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
		break;
	case 1:
		desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
		break;
	case 2:
		desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
		break;
	default:
		return S_FALSE;
	}

	switch (fillMode)
	{
	case 0:
		desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		break;
	case 1:
		desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
		break;
	default:
		return S_FALSE;
	}

	ID3D11RasterizerState* rsState = nullptr;
	CHECK_HR(m_GameContext.pDevice->CreateRasterizerState(&desc, &rsState));
	m_GameContext.pDeviceContext->RSSetState(rsState);

	rsState->Release();

	return S_OK;
}

void SpringWind::UpdatePPTargets(SpringWind::RenderTarget& rt, const UINT width, const UINT height)
{
	SafeRelease(rt.m_pColor);
	SafeRelease(rt.m_pRenderTargetView);
	SafeRelease(rt.m_pDepthStencil);
	SafeRelease(rt.m_pDepthStencilView);
	SafeRelease(rt.m_pShaderResourceView);
	//RESOURCE
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;


	CHECK_HR(m_GameContext.pDevice->CreateTexture2D(&textureDesc, nullptr, &rt.m_pColor));

	CHECK_HR(m_GameContext.pDevice->CreateRenderTargetView(rt.m_pColor, nullptr, &rt.m_pRenderTargetView));

	CHECK_HR(m_GameContext.pDevice->CreateShaderResourceView(rt.m_pColor, nullptr, &rt.m_pShaderResourceView));

	ZeroMemory(&textureDesc, sizeof(textureDesc));

	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc.MiscFlags = 0;

	CHECK_HR(m_GameContext.pDevice->CreateTexture2D(&textureDesc, nullptr, &rt.m_pDepthStencil));

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));

	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	descDSV.Flags = 0;

	CHECK_HR(m_GameContext.pDevice->CreateDepthStencilView(rt.m_pDepthStencil, &descDSV, &rt.m_pDepthStencilView));
}

HRESULT SpringWind::Render(void * pResource, bool isNewSurface)
{
	// If we've gotten a new Surface, need to initialize the renderTarget.
	// One of the times that this happens is on a resize.
	if (isNewSurface)
	{
		m_GameContext.pDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
		SafeRelease(m_MainRT.m_pRenderTargetView);
		SafeRelease(m_MainRT.m_pShaderResourceView);
		CHECK_HR(InitRenderTarget(pResource));

		SetCameraAspectRatio(m_Width / (float)m_Height);

		UpdatePPTargets(m_MainPPRT, m_Width, m_Height);
	}

	// Update our time
	m_GameContext.pGameTime->Update();
	// Clear the back buffer
	static float ClearColor[4] = { 0.0f, 0.2f, 0.2f, 1.0f };
	m_GameContext.pDeviceContext->ClearRenderTargetView(m_MainRT.m_pRenderTargetView, ClearColor);
	m_GameContext.pDeviceContext->ClearDepthStencilView(m_MainRT.m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_pScene->Draw(m_GameContext);

	m_GameContext.pDeviceContext->Flush();

	return S_OK;
}