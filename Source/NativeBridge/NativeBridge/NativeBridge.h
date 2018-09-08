#pragma once

#include "Interfaces/INativeBridge.h"

namespace SpringWindEngine
{
	class SpringWindWindow;
	class NativeWindowBridge;
}

namespace ShaderBoxNativeBridge
{
	class NativeBridge final : public INativeBridge
	{
	public:
		void InitEngine() override;
		void ShutdownEngine() override;

		INativeWindowBridge* RegisterWindow(HWND parentHandle, REGISTER_WINDOW_CALLBACK callback) override;
		void UnregisterWindow(INativeWindowBridge* pWindowBridge, REGISTER_WINDOW_CALLBACK callback) override;

		LRESULT WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool& handled) override;

		void CreateThumbnailModel(const std::wstring& modelPath, const std::wstring& saveLocation, REGISTER_WINDOW_CALLBACK callback) override;

		static NativeBridge* GetInstance() { return s_Instance; }

		void SetThumbnailRenderTarget();
		const SpringWindEngine::RenderTarget& GetThumbRT() const { return m_ThumbRT; }
		const SpringWindEngine::RenderTarget& GetPPThumbRT() const { return m_PPThumbRT; }
		ID3D11Texture2D* GetThumbTexture() const { return m_pThumbTexture; }
		constexpr UINT GetThumbSize() const { return THUMB_SIZE; }
	private:
		void Init();
		void InitThumbnailRenderTarget();
		void CreateThumbnailScene();

		std::thread* m_pEngineThread;
		std::mutex m_Mutex;
		std::condition_variable m_CV;

		std::unordered_map<HWND, SpringWindEngine::SpringWindWindow*> m_WindowPtrMap;

		SpringWindEngine::MeshDrawComponent* m_pMeshDrawComponent = nullptr;
		SpringWindEngine::GameScene* m_pScene = nullptr;

		static NativeBridge* s_Instance;

		static constexpr const UINT THUMB_SIZE = 200;

		SpringWindEngine::RenderTarget m_ThumbRT;
		SpringWindEngine::RenderTarget m_PPThumbRT;

		ID3D11Texture2D* m_pThumbTexture = nullptr;
	};
}
