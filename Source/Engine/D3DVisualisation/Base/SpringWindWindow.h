#pragma once

namespace SpringWindEngine
{
	class SpringWindWindow final
	{
	public:
		SpringWindWindow();
		~SpringWindWindow();

		void Initialize(const HINSTANCE hInstance, const HWND parentHandle, const LPCTSTR windowName);
		void Shutdown();

		static LRESULT CALLBACK WindowProcedureStatic(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		LRESULT WindowProcedure(const HWND hWnd, const UINT message, const WPARAM wParam, const LPARAM lParam, bool& handled);

		void Render(const EngineContext& engineContext);

		GameScene* GetScene() const { return m_pScene; }
		void SetScene(GameScene* pScene);

		const WindowContext& GetWindowContext() const { return m_WindowContext; }

		const HWND GetHandle() const { return m_pWindowHandle; }

		UINT GetWidth() const { return m_WindowContext.Width; }
		UINT GetHeight() const { return m_WindowContext.Height; }

		const RenderTarget& GetRenderTarget() const { return m_MainRT; }
		const RenderTarget& GetPPRenderTarget() const { return m_PPRT; }

		void SetUpViewport();
	private:
		void InitRenderTarget();
		void CreateBuffers();

		HWND m_pWindowHandle = nullptr;
		IDXGISwapChain1* m_pSwapChain = nullptr;

		static constexpr UINT SWAPCHAIN_BUFFERCOUNT = 2;
		static constexpr DXGI_FORMAT SWAPCHAIN_FORMAT = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;

		D3D11_VIEWPORT m_Viewport = {};

		GameScene* m_pScene = nullptr;
		WindowContext m_WindowContext;

		RenderTarget m_MainRT;

		RenderTarget m_PPRT;
	};
}
