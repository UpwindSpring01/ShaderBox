#include "stdafx.h"

#include "SpringWindWindow.h"

SpringWindEngine::SpringWindWindow::SpringWindWindow()
{
}

SpringWindEngine::SpringWindWindow::~SpringWindWindow()
{
}

void SpringWindEngine::SpringWindWindow::Initialize(const HINSTANCE hInstance, const HWND parentHandle, const LPCTSTR windowName)
{
	////////////////////////
	// CREATE WINDOW
	////////////////////////
	DWORD style = WS_POPUP;
	if (parentHandle != nullptr)
	{
		style = WS_CHILD;
	}
	m_pWindowHandle = CreateWindow(SpringWind::GetWindowClassName(),
		windowName,
		style,
		0,
		0,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		parentHandle,
		nullptr,
		hInstance,
		this);

	ShowWindow(m_pWindowHandle, SW_SHOW);

	////////////////////////
	// CREATE SWAPCHAIN
	////////////////////////
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = 0;
	swapChainDesc.Height = 0;
	swapChainDesc.Format = SWAPCHAIN_FORMAT;
	swapChainDesc.Stereo = false;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	swapChainDesc.BufferCount = SWAPCHAIN_BUFFERCOUNT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = 0;

	const EngineContext& context = SpringWind::GetInstance()->GetEngineContext();
	CHECK_HR(context.pFactory->CreateSwapChainForHwnd(context.pDevice, m_pWindowHandle, &swapChainDesc, nullptr, nullptr, &m_pSwapChain));

	////////////////////////
	// INITIALIZE GAME
	////////////////////////
	m_WindowContext.pGameTime = new GameTime();
	m_WindowContext.pGameTime->Reset();
	m_WindowContext.pGameTime->Start();

	ID3D11Texture2D* pBackbuffer;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackbuffer));
	D3D11_TEXTURE2D_DESC desc;
	pBackbuffer->GetDesc(&desc);
	m_WindowContext.Width = desc.Width;
	m_WindowContext.Height = desc.Height;
	pBackbuffer->Release();

	InitRenderTarget();
	CreateBuffers();
}

void SpringWindEngine::SpringWindWindow::Shutdown()
{
	delete m_WindowContext.pGameTime;

	m_MainRT.Release();
	m_PPRT.Release();

	m_pSwapChain->Release();
	m_pSwapChain = nullptr;
	m_pScene = nullptr;

	DestroyWindow(m_pWindowHandle);
}

void SpringWindEngine::SpringWindWindow::CreateBuffers()
{
	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth = { 0 };
	descDepth.Width = m_WindowContext.Width;
	descDepth.Height = m_WindowContext.Height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	ID3D11Texture2D* pDepthBuffer;
	CHECK_HR(SpringWind::GetInstance()->GetEngineContext().pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthBuffer));

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	CHECK_HR(SpringWind::GetInstance()->GetEngineContext().pDevice->CreateDepthStencilView(pDepthBuffer, &descDSV, &m_MainRT.pDepthStencilView));
	pDepthBuffer->Release();
}

void SpringWindEngine::SpringWindWindow::Render(const EngineContext& engineContext)
{
	if (SpringWind::GetInstance()->GetLastRenderedWindow() != this)
	{
		SetUpViewport();
	}

	engineContext.pDeviceContext->OMSetRenderTargets(1, &m_MainRT.pRenderTargetView, m_MainRT.pDepthStencilView);

	// Update our time
	m_WindowContext.pGameTime->Update();
	// Clear the back buffer
	static float ClearColor[4] = { 0.0f, 0.2f, 0.2f, 1.0f };
	engineContext.pDeviceContext->ClearRenderTargetView(m_MainRT.pRenderTargetView, ClearColor);
	engineContext.pDeviceContext->ClearDepthStencilView(m_MainRT.pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_pScene->Draw(engineContext, m_WindowContext, m_MainRT, m_PPRT);

	m_pSwapChain->Present(1, 0);
}

void SpringWindEngine::SpringWindWindow::SetScene(GameScene* pScene)
{
	assert(pScene->GetWindow() == nullptr);
	pScene->SetWindow(this);
	m_pScene = pScene;
}

void SpringWindEngine::SpringWindWindow::SetUpViewport()
{
	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (float)m_WindowContext.Width;
	vp.Height = (float)m_WindowContext.Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	SpringWind::GetInstance()->GetEngineContext().pDeviceContext->RSSetViewports(1, &vp);
}

void SpringWindEngine::SpringWindWindow::InitRenderTarget()
{

	ID3D11Texture2D* pBackbuffer;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackbuffer));

	const EngineContext& engineContext = SpringWind::GetInstance()->GetEngineContext();

	D3D11_RENDER_TARGET_VIEW_DESC rtDesc = { 0 };
	rtDesc.Format = SWAPCHAIN_FORMAT;
	rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtDesc.Texture2D.MipSlice = 0;

	CHECK_HR(engineContext.pDevice->CreateRenderTargetView(pBackbuffer, &rtDesc, &m_MainRT.pRenderTargetView));
	CHECK_HR(engineContext.pDevice->CreateShaderResourceView(pBackbuffer, nullptr, &m_MainRT.pShaderResourceView));
	pBackbuffer->Release();

	D3D11_TEXTURE2D_DESC textureDesc = { 0 };
	textureDesc.Width = m_WindowContext.Width;
	textureDesc.Height = m_WindowContext.Height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = SWAPCHAIN_FORMAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	ID3D11Texture2D* pTexture = nullptr;
	CHECK_HR(engineContext.pDevice->CreateTexture2D(&textureDesc, nullptr, &pTexture));
	CHECK_HR(engineContext.pDevice->CreateRenderTargetView(pTexture, nullptr, &m_PPRT.pRenderTargetView));
	CHECK_HR(engineContext.pDevice->CreateShaderResourceView(pTexture, nullptr, &m_PPRT.pShaderResourceView));
	pTexture->Release();
}

LRESULT CALLBACK SpringWindEngine::SpringWindWindow::WindowProcedureStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_CREATE)
	{
		CREATESTRUCT *pCS = reinterpret_cast<CREATESTRUCT*>(lParam);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCS->lpCreateParams));
	}
	else
	{
		SpringWindWindow* pThisWindow = reinterpret_cast<SpringWindWindow*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
		if (pThisWindow)
		{
			bool handled = false;
			LRESULT result = pThisWindow->WindowProcedure(hWnd, message, wParam, lParam, handled);
			if (handled)
			{
				return result;
			}
		}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT SpringWindEngine::SpringWindWindow::WindowProcedure(const HWND /*hWnd*/, const UINT message, const WPARAM /*wParam*/, const LPARAM lParam, bool& handled)
{
	switch (message)
	{
	case WM_DESTROY:
		handled = true;
	case WM_SIZE:
		m_WindowContext.Width = LOWORD(lParam);
		m_WindowContext.Height = HIWORD(lParam);
		if (m_pScene && m_pScene->GetActiveCamera())
		{
			m_pScene->GetActiveCamera()->SetProjectionDirty();
		}
		if (m_pSwapChain)
		{
			const SpringWind* engine = SpringWind::GetInstance();
			const EngineContext& engineContext = engine->GetEngineContext();
			if (engine->GetLastRenderedWindow() == this)
			{
				engineContext.pDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
			}

			m_MainRT.Release();
			m_PPRT.Release();

			CHECK_HR(m_pSwapChain->ResizeBuffers(SWAPCHAIN_BUFFERCOUNT, m_WindowContext.Width, m_WindowContext.Height, DXGI_FORMAT::DXGI_FORMAT_UNKNOWN, 0));

			InitRenderTarget();
			CreateBuffers();

			if (engine->GetLastRenderedWindow() == this)
			{
				SetUpViewport();
			}
		}
	}
	return 0;
}