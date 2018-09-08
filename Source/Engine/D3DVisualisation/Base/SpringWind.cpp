#include "stdafx.h"
#include "SpringWind.h"
#include "SpringWindWindow.h"

#include "Graphics/SpriteRenderer.h"
#include "SceneGraph/SceneManager.h"

#include "Content/ContentManager.h"
#include "Content/Shaders/EffectFactory.h"
#include "Content/Shaders/ShaderStorage.h"

#include "Containers/ThreadSafeFunctionContainer.h"

#include <fstream>
#include <iostream>

#include <dwmapi.h>
#pragma comment(lib, "Dwmapi.lib")

SpringWindEngine::SpringWind* SpringWindEngine::SpringWind::m_pInstance = nullptr;

SpringWindEngine::SpringWind::SpringWind()
{
#ifdef _DEBUG
	if (m_pInstance != nullptr)
	{
		Logger::LogError(L"Can not instantiate more then one engine.");
	}
#endif // _DEBUG

	m_pInstance = this;
}


SpringWindEngine::SpringWind::~SpringWind()
{
	m_pInstance = nullptr;
}

void SpringWindEngine::SpringWind::Shutdown()
{
	m_IsRunning = false;
}

void SpringWindEngine::SpringWind::ShutdownInternal()
{
	assert(m_WindowPtrArr.size() == 0);

	UnregisterClass(GetWindowClassName(), m_pHInstance);

	ShaderStorage::GetInstance()->UnloadVertexShader(m_pPPVertexShader, nullptr);

	SceneManager::DestroyInstance();

	ContentManager::Shutdown();
	
	SpriteRenderer::DestroyInstance();
	EffectFactory::DestroyInstance();
	ShaderStorage::DestroyInstance();

	for (int i = 0; i < SAMPLER_AMOUNT; ++i)
	{
		m_pSamplersArr[i]->Release();
	}

	m_pBaseCBufferFrameBuffer->Release();
	m_pBaseCBufferObjectBuffer->Release();

	delete m_EngineContext.pBaseCBufferFrame;
	delete m_EngineContext.pBaseCBufferObject;

	SafeRelease(m_EngineContext.pFactory);
	SafeRelease(m_EngineContext.pDeviceContext);
	SafeRelease(m_EngineContext.pDevice);
}

HRESULT SpringWindEngine::SpringWind::Initialize(HINSTANCE hInstance)
{
	m_pHInstance = hInstance;
	HRESULT hr = S_OK;

	////////////////////////
	// CREATE FACTORY
	////////////////////////
	#ifdef _DEBUG
		CHECK_HR(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, __uuidof(IDXGIFactory2), reinterpret_cast<void**>(&m_EngineContext.pFactory)))
	#else
		CreateDXGIFactory2(0, __uuidof(IDXGIFactory2), reinterpret_cast<void**>(&m_EngineContext.pFactory));
	#endif

	////////////////////////
	// CREATE DEVICE + DEVICECONTEXT
	////////////////////////
	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

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
		hr = D3D11CreateDevice(nullptr, driverTypes[driverTypeIndex], nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &m_EngineContext.pDevice, &m_FeatureLevel, &m_EngineContext.pDeviceContext);

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
	// REGISTER WINDOW CLASS
	////////////////////////
	WNDCLASS windowClass = {};
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.hIcon = nullptr; // LoadIcon(0, IDI_APPLICATION);
	windowClass.hbrBackground = nullptr; // (HBRUSH)GetStockObject(WHITE_BRUSH);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = SpringWindWindow::WindowProcedureStatic;
	windowClass.hInstance = hInstance;
	windowClass.lpszClassName = GetWindowClassName();

	RegisterClass(&windowClass);

	////////////////////////
	// 
	////////////////////////
	SetDefaultSamplers();
	InitFrameAndObjectBuffers();

	////////////////////////
	// INITIALIZE GAME
	////////////////////////

	ContentManager::Initialize();

	ShaderStorage::CreateInstance();
	EffectFactory::CreateInstance();
	SpriteRenderer::CreateInstance();

	SceneManager::CreateInstance();

	m_pPPVertexShader = ShaderStorage::GetInstance()->GetVertexShader(L"./Resources/.Internal/CSO/PP_VS.cso", nullptr);
	return hr;
}

void SpringWindEngine::SpringWind::RegisterWindow(SpringWindWindow* pWindow, const HINSTANCE hInstance, const HWND parentHandle, const LPCTSTR windowName)
{
	pWindow->Initialize(hInstance, parentHandle, windowName);
	m_WindowPtrArr.push_back(pWindow);
}

void SpringWindEngine::SpringWind::UnregisterWindow(SpringWindWindow* pWindow)
{
	m_WindowPtrArr.erase(std::remove(m_WindowPtrArr.begin(), m_WindowPtrArr.end(), pWindow));
	pWindow->Shutdown();
}

void SpringWindEngine::SpringWind::StartGameLoop()
{
	m_IsRunning = true;

	MSG msg = {};
	while (true)
	{
		m_ThreadSafeFunctionContainer.ExecuteAll();
		if (!m_IsRunning)
		{
			break;
		}

		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		for(SpringWindWindow* pWindow : m_WindowPtrArr)
		{
			pWindow->Render(m_EngineContext);
			m_pLastRenderedWindow = pWindow;
		}
	}

	ShutdownInternal();
}

void SpringWindEngine::SpringWind::SetDefaultSamplers()
{
	ID3D11Device* pDevice = m_EngineContext.pDevice;
	ID3D11DeviceContext* pDeviceContext = m_EngineContext.pDeviceContext;

	D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());
	samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	CHECK_HR(pDevice->CreateSamplerState(&samplerDesc, &m_pSamplersArr[0]));

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_MIRROR;
	CHECK_HR(pDevice->CreateSamplerState(&samplerDesc, &m_pSamplersArr[1]));

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
	CHECK_HR(pDevice->CreateSamplerState(&samplerDesc, &m_pSamplersArr[2]));

	samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	CHECK_HR(pDevice->CreateSamplerState(&samplerDesc, &m_pSamplersArr[3]));

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_MIRROR;
	CHECK_HR(pDevice->CreateSamplerState(&samplerDesc, &m_pSamplersArr[4]));

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
	CHECK_HR(pDevice->CreateSamplerState(&samplerDesc, &m_pSamplersArr[5]));

	pDeviceContext->VSSetSamplers(0, SAMPLER_AMOUNT, m_pSamplersArr);
	pDeviceContext->HSSetSamplers(0, SAMPLER_AMOUNT, m_pSamplersArr);
	pDeviceContext->DSSetSamplers(0, SAMPLER_AMOUNT, m_pSamplersArr);
	pDeviceContext->GSSetSamplers(0, SAMPLER_AMOUNT, m_pSamplersArr);
	pDeviceContext->PSSetSamplers(0, SAMPLER_AMOUNT, m_pSamplersArr);
}

void SpringWindEngine::SpringWind::InitFrameAndObjectBuffers()
{
	ID3D11Device* pDevice = m_EngineContext.pDevice;

	// Initialize buffer for cbuffer variables, which stays the same for al draw calls during one frame
	D3D11_BUFFER_DESC constantBufferDesc = {};
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.ByteWidth = RoundUpMultiple16(sizeof(BaseConstantBufferFrame));
	CHECK_HR(pDevice->CreateBuffer(
		&constantBufferDesc,
		nullptr,             // leave the buffer uninitialized
		&m_pBaseCBufferFrameBuffer
	));
	m_EngineContext.pBaseCBufferFrame = new BaseConstantBufferFrame();

	constantBufferDesc.ByteWidth = sizeof(BaseConstantBufferObject);
	CHECK_HR(pDevice->CreateBuffer(
		&constantBufferDesc,
		nullptr,             // leave the buffer uninitialized
		&m_pBaseCBufferObjectBuffer
	));
	m_EngineContext.pBaseCBufferObject = new BaseConstantBufferObject();

	BindBuffers();
}

void SpringWindEngine::SpringWind::BindBuffers()
{
	ID3D11DeviceContext* pDeviceContext = m_EngineContext.pDeviceContext;
	pDeviceContext->VSSetConstantBuffers(0, 1, &m_pBaseCBufferFrameBuffer);
	pDeviceContext->VSSetConstantBuffers(1, 1, &m_pBaseCBufferObjectBuffer);

	pDeviceContext->HSSetConstantBuffers(0, 1, &m_pBaseCBufferFrameBuffer);
	pDeviceContext->HSSetConstantBuffers(1, 1, &m_pBaseCBufferObjectBuffer);

	pDeviceContext->DSSetConstantBuffers(0, 1, &m_pBaseCBufferFrameBuffer);
	pDeviceContext->DSSetConstantBuffers(1, 1, &m_pBaseCBufferObjectBuffer);

	pDeviceContext->GSSetConstantBuffers(0, 1, &m_pBaseCBufferFrameBuffer);
	pDeviceContext->GSSetConstantBuffers(1, 1, &m_pBaseCBufferObjectBuffer);

	pDeviceContext->PSSetConstantBuffers(0, 1, &m_pBaseCBufferFrameBuffer);
	pDeviceContext->PSSetConstantBuffers(1, 1, &m_pBaseCBufferObjectBuffer);
}

void SpringWindEngine::SpringWind::UpdateObjectBuffer()
{
	D3D11_MAPPED_SUBRESOURCE resource;
	m_EngineContext.pDeviceContext->Map(m_pBaseCBufferObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, m_EngineContext.pBaseCBufferObject, sizeof(BaseConstantBufferObject));
	m_EngineContext.pDeviceContext->Unmap(m_pBaseCBufferObjectBuffer, 0);
}

void SpringWindEngine::SpringWind::UpdateFrameBuffer()
{
	D3D11_MAPPED_SUBRESOURCE resource;
	m_EngineContext.pDeviceContext->Map(m_pBaseCBufferFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, m_EngineContext.pBaseCBufferFrame, sizeof(BaseConstantBufferFrame));
	m_EngineContext.pDeviceContext->Unmap(m_pBaseCBufferFrameBuffer, 0);
}