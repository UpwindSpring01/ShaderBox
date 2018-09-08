#pragma once

#include "Containers/ThreadSafeFunctionContainer.h"

namespace SpringWindEngine
{
	class SpringWindWindow;
	struct EngineContext;

	class SpringWind final
	{
	public:
		SpringWind();
		virtual ~SpringWind();

		static SpringWind* GetInstance() { return m_pInstance; }

		static constexpr LPCTSTR GetWindowClassName(){ return L"SpringWindEngineClassName"; }

		void RegisterWindow(SpringWindWindow* pWindow, const HINSTANCE hInstance, const HWND parentHandle, const LPCTSTR windowName);
		void UnregisterWindow(SpringWindWindow* pWindow);

		HRESULT Initialize(const HINSTANCE hInstance);
		void Shutdown();

		void StartGameLoop();

		ThreadSafeFunctionContainer& GetThreadSafeFunctionContainer() { return m_ThreadSafeFunctionContainer; }
		const EngineContext& GetEngineContext() const { return m_EngineContext; }

		void UpdateObjectBuffer();
		void UpdateFrameBuffer();

		const CameraComponent* GetLastRenderedCamera() const { return m_pLastRenderedCamera; }
		void SetLastRenderedCamera(const CameraComponent* pCamera) { m_pLastRenderedCamera = pCamera; }

		ID3D11VertexShader* GetPPVertexShader() const { return m_pPPVertexShader; }

		const SpringWindWindow* GetLastRenderedWindow() const { return m_pLastRenderedWindow; }
		void ResetLastRenderedWindow() { m_pLastRenderedWindow = nullptr; }
		const std::vector<SpringWindWindow*>& GetWindows() const { return m_WindowPtrArr; }

	private:
		void SetDefaultSamplers();
		void InitFrameAndObjectBuffers();
		void BindBuffers();

		void ShutdownInternal();
		
		EngineContext m_EngineContext;

		static SpringWind* m_pInstance;
		HINSTANCE m_pHInstance = nullptr;

		std::vector<SpringWindWindow*> m_WindowPtrArr;
		bool m_IsRunning = false;

		static constexpr UINT SAMPLER_AMOUNT = 6;
		ID3D11SamplerState* m_pSamplersArr[SAMPLER_AMOUNT];

		ID3D11Buffer* m_pBaseCBufferFrameBuffer;
		ID3D11Buffer* m_pBaseCBufferObjectBuffer;

		ID3D11VertexShader* m_pPPVertexShader = nullptr;

		ThreadSafeFunctionContainer m_ThreadSafeFunctionContainer;

		D3D_FEATURE_LEVEL m_FeatureLevel;

		const CameraComponent* m_pLastRenderedCamera;
		const SpringWindWindow* m_pLastRenderedWindow;

		SpringWind(const SpringWind& t) = delete;
		SpringWind& operator=(const SpringWind& t) = delete;
	};
}