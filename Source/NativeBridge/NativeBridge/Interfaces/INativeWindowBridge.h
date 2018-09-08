#pragma once

typedef void(__stdcall *REGISTER_WINDOW_CALLBACK)();

namespace SpringWindEngine
{
	class SpringWindWindow;
}

namespace ShaderBoxNativeBridge
{
	class __declspec(dllexport) INativeWindowBridge abstract
	{
	public:
		virtual ~INativeWindowBridge() {};

		virtual SpringWindEngine::SpringWindWindow* GetWindow() const = 0;

		virtual HWND GetWindowHandle() const = 0;
		virtual void SetModel(const std::wstring& filePath) const = 0;

		virtual void SetShaders(const std::wstring& vertexShader, const std::wstring& hullShader, const std::wstring& domainShader,
			const std::wstring& geometryShader, const std::wstring& pixelShader) = 0;
		virtual void SetPPShader(const std::wstring& pixelShader) = 0;

		virtual void UpdateShaders(const std::wstring& vertexShader, const std::wstring& hullShader, const std::wstring& domainShader,
			const std::wstring& geometryShader, const std::wstring& pixelShader) = 0;
		virtual void UpdatePPShader(const std::wstring& pixelShader) = 0;

		virtual void SetTopology(const int topology) = 0;
		virtual void SetRasterizerState(const int cullMode, const int fillMode) = 0;
		virtual void SetCameraOffset(const float offset) = 0;

		virtual void UpdateBuffer(const UINT slotIndex, const int shaderType, const std::byte* data, const UINT length) = 0;
		virtual void UpdateImageBuffer(const UINT slotIndex, const int shaderType, const std::wstring& path) = 0;

		virtual void UpdatePPBuffer(const UINT slotIndex, const std::byte* data, const UINT length) = 0;
		virtual void UpdatePPImageBuffer(const UINT slotIndex, const std::wstring& path) = 0;

		virtual void RenderThumbnail(const std::wstring& saveLocation, REGISTER_WINDOW_CALLBACK callback) = 0;
	};
}