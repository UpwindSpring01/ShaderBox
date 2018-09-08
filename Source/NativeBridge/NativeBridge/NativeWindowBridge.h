#pragma once

#include "Interfaces/INativeWindowBridge.h"

typedef void(__stdcall *REGISTER_WINDOW_CALLBACK)();

namespace ShaderBoxNativeBridge
{
	class ShaderBoxMaterial;
	class ShaderBoxPPMaterial;

	class NativeWindowBridge final : public INativeWindowBridge
	{
	public:
		NativeWindowBridge(HWND parentHandle, REGISTER_WINDOW_CALLBACK callback);
		~NativeWindowBridge();

		virtual SpringWindEngine::SpringWindWindow* GetWindow() const override { return m_pWindow; }

		HWND GetWindowHandle() const override;
		void SetModel(const std::wstring& filePath) const override;

		void SetShaders(const std::wstring& vertexShader, const std::wstring& hullShader,
			const std::wstring& domainShader, const std::wstring& geometryShader, const std::wstring& pixelShader) override;
		void SetPPShader(const std::wstring& pixelShader) override;

		void UpdateShaders(const std::wstring& vertexShader, const std::wstring& hullShader,
			const std::wstring& domainShader, const std::wstring& geometryShader, const std::wstring& pixelShader) override;
		void UpdatePPShader(const std::wstring& pixelShader) override;

		void SetTopology(const int topology) override;
		void SetRasterizerState(const int cullMode, const int fillMode) override;
		void SetCameraOffset(const float offset) override;

		void UpdateBuffer(const UINT slotIndex, const int shaderType, const std::byte* data, const UINT length) override;
		void UpdateImageBuffer(const UINT slotIndex, const int shaderType, const std::wstring& path) override;

		void UpdatePPBuffer(const UINT slotIndex, const std::byte* data, const UINT length) override;
		void UpdatePPImageBuffer(const UINT slotIndex, const std::wstring& path) override;

		void RenderThumbnail(const std::wstring& saveLocation, REGISTER_WINDOW_CALLBACK callback) override;
	private:
		SpringWindEngine::SpringWindWindow* m_pWindow = nullptr;

		SpringWindEngine::MeshDrawComponent* m_pMeshDrawComponent = nullptr;
		SpringWindEngine::CameraComponent* m_pCamComponent = nullptr;
		SpringWindEngine::Material* m_pDefaultMaterial = nullptr;
		ShaderBoxNativeBridge::ShaderBoxMaterial* m_pActiveMaterial = nullptr;
		ShaderBoxNativeBridge::ShaderBoxPPMaterial* m_pActivePPMaterial = nullptr;
	};
}
