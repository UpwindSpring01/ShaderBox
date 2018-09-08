#include "stdafx.h"

#include "NativeBridge.h"
#include "NativeWindowBridge.h"

#include "Base/SpringWindWindow.h"
#include "SceneGraph/SceneManager.h"
#include "Content/Shaders/EffectFactory.h"
#include "Containers/ThreadSafeFunctionContainer.h"

using namespace SpringWindEngine;

ShaderBoxNativeBridge::NativeBridge* ShaderBoxNativeBridge::NativeBridge::s_Instance = nullptr;

void ShaderBoxNativeBridge::NativeBridge::InitEngine()
{
	s_Instance = this;
	m_pEngineThread = new std::thread(&ShaderBoxNativeBridge::NativeBridge::Init, this);
}

void ShaderBoxNativeBridge::NativeBridge::ShutdownEngine()
{
	SpringWind::GetInstance()->GetThreadSafeFunctionContainer().Push([this]
	{
		delete m_pMeshDrawComponent->GetMaterial();

		m_ThumbRT.Release();
		m_PPThumbRT.Release();

		SafeRelease(m_pThumbTexture);

		SpringWind::GetInstance()->Shutdown();
	});

	m_pEngineThread->join();
	delete m_pEngineThread;

	delete SpringWind::GetInstance();
}

ShaderBoxNativeBridge::INativeWindowBridge* ShaderBoxNativeBridge::NativeBridge::RegisterWindow(HWND parentHandle, REGISTER_WINDOW_CALLBACK callback)
{
	return new NativeWindowBridge(parentHandle, callback);
}

void ShaderBoxNativeBridge::NativeBridge::UnregisterWindow(INativeWindowBridge* pWindowBridge, REGISTER_WINDOW_CALLBACK callback)
{
	SpringWind::GetInstance()->GetThreadSafeFunctionContainer().Push([pWindowBridge, callback]
	{
		SpringWind::GetInstance()->UnregisterWindow(pWindowBridge->GetWindow());
		delete pWindowBridge;

		callback();
	});
}

void ShaderBoxNativeBridge::NativeBridge::Init()
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
	SetThreadDescription(GetCurrentThread(), L"ENGINE_THREAD");

	SpringWind* pEngine = new SpringWind();
	pEngine->Initialize(GetModuleHandle(nullptr));
	InitThumbnailRenderTarget();
	CreateThumbnailScene();

	pEngine->StartGameLoop();
}

LRESULT ShaderBoxNativeBridge::NativeBridge::WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool& handled)
{
	bool executed = false;
	LRESULT result;

	SpringWind::GetInstance()->GetThreadSafeFunctionContainer().Push([&result, &handled, &executed, this, hWnd, message, wParam, lParam]
	{
		{
			std::lock_guard<std::mutex> lock(m_Mutex);
			SpringWindWindow* pThisWindow = reinterpret_cast<SpringWindWindow*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
			if (pThisWindow)
			{
				result = pThisWindow->WindowProcedure(hWnd, message, wParam, lParam, handled);
			}
			executed = true;
		}
		m_CV.notify_all();
	});

	std::unique_lock<std::mutex> lock(m_Mutex);
	m_CV.wait(lock, [&executed] { return executed; });

	return result;
}

void ShaderBoxNativeBridge::NativeBridge::InitThumbnailRenderTarget()
{
	const EngineContext& engineContext = SpringWind::GetInstance()->GetEngineContext();

	//RESOURCE
	D3D11_TEXTURE2D_DESC textureDesc = { 0 };

	textureDesc.Width = THUMB_SIZE;
	textureDesc.Height = THUMB_SIZE;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	CHECK_HR(engineContext.pDevice->CreateTexture2D(&textureDesc, nullptr, &m_pThumbTexture));
	CHECK_HR(engineContext.pDevice->CreateRenderTargetView(m_pThumbTexture, nullptr, &m_ThumbRT.pRenderTargetView));
	CHECK_HR(engineContext.pDevice->CreateShaderResourceView(m_pThumbTexture, nullptr, &m_ThumbRT.pShaderResourceView));

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth = { 0 };
	descDepth.Width = THUMB_SIZE;
	descDepth.Height = THUMB_SIZE;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	ID3D11Texture2D* pThumbTexture = nullptr;
	CHECK_HR(engineContext.pDevice->CreateTexture2D(&descDepth, nullptr, &pThumbTexture));

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = { 0 };
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	CHECK_HR(engineContext.pDevice->CreateDepthStencilView(pThumbTexture, &descDSV, &m_ThumbRT.pDepthStencilView));
	pThumbTexture->Release();

	// PP target
	CHECK_HR(engineContext.pDevice->CreateTexture2D(&textureDesc, nullptr, &pThumbTexture));
	CHECK_HR(engineContext.pDevice->CreateRenderTargetView(pThumbTexture, nullptr, &m_PPThumbRT.pRenderTargetView));
	CHECK_HR(engineContext.pDevice->CreateShaderResourceView(pThumbTexture, nullptr, &m_PPThumbRT.pShaderResourceView));
	pThumbTexture->Release();
}

void ShaderBoxNativeBridge::NativeBridge::SetThumbnailRenderTarget()
{
	const EngineContext& engineContext = SpringWind::GetInstance()->GetEngineContext();
	engineContext.pDeviceContext->OMSetRenderTargets(1, &m_ThumbRT.pRenderTargetView, m_ThumbRT.pDepthStencilView);

	D3D11_VIEWPORT vp;
	vp.Width = (float)THUMB_SIZE;
	vp.Height = (float)THUMB_SIZE;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	engineContext.pDeviceContext->RSSetViewports(1, &vp);

	static float ClearColor[4] = { 0.0f, 0.2f, 0.2f, 1.0f };
	engineContext.pDeviceContext->ClearRenderTargetView(m_ThumbRT.pRenderTargetView, ClearColor);
	engineContext.pDeviceContext->ClearDepthStencilView(m_ThumbRT.pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void ShaderBoxNativeBridge::NativeBridge::CreateThumbnailScene()
{
	SpringWind::GetInstance()->GetThreadSafeFunctionContainer().Push([this]
	{
		GameObject* pCamera = new GameObject();
		CameraComponent* pCamComponent = new CameraComponent();
		pCamComponent->SetAspectRatio(1.0f);
		pCamera->AddComponent(pCamComponent);

		XMVECTOR worldPosition = XMVectorSet(0, 5, -5, 0);
		
		XMVECTOR upVec = XMVectorSet(0, 1, 0, 0);
		XMVECTOR eyeDir = XMVectorSet(0, -5, 5, 0);

		XMFLOAT4 rot;
		XMStoreFloat4(&rot, XMQuaternionRotationMatrix(XMMatrixInverse(nullptr, XMMatrixLookToLH(worldPosition, eyeDir, upVec))));

		pCamera->GetTransform()->Translation(0, 5, -5);
		pCamera->GetTransform()->Rotation(rot);

		m_pScene = SceneManager::GetInstance()->CreateScene();
		m_pScene->AddChild(pCamera);
		m_pScene->SetActiveCamera(pCamComponent);

		GameObject* obj = new GameObject();
		m_pMeshDrawComponent = new MeshDrawComponent();

		Effect* pEffect = EffectFactory::GetInstance()->LoadEffect(L"./Resources/.Internal/CSO/vs.cso", L"./Resources/.Internal/CSO/ps.cso");
		pEffect->Topology = D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		Material* pMaterial = new Material(pEffect);

		m_pMeshDrawComponent->SetMaterial(pMaterial);

		obj->AddComponent(m_pMeshDrawComponent);
		m_pScene->AddChild(obj);
	});
}

void ShaderBoxNativeBridge::NativeBridge::CreateThumbnailModel(const std::wstring& modelPath, const std::wstring& saveLocation, REGISTER_WINDOW_CALLBACK callback)
{
	SpringWind::GetInstance()->GetThreadSafeFunctionContainer().Push([this, modelPath, saveLocation, callback]
	{
		const EngineContext& engineContext = SpringWind::GetInstance()->GetEngineContext();

		m_pMeshDrawComponent->SetMesh(ContentManager::Load<Mesh>(modelPath));

		SetThumbnailRenderTarget();

		WindowContext context;
		context.Width = GetThumbSize();
		context.Height = GetThumbSize();
		context.pGameTime = nullptr;
		m_pScene->Draw(engineContext, context, GetThumbRT(), GetPPThumbRT());
		CHECK_HR(SaveWICTextureToFile(engineContext.pDeviceContext, GetThumbTexture(), GUID_ContainerFormatPng, saveLocation.c_str()));

		// Reset
		SpringWind::GetInstance()->ResetLastRenderedWindow();
		callback();
	});
}