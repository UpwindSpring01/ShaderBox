#pragma once

using namespace System;
using namespace System::Windows::Interop;
using namespace System::Runtime::InteropServices;

namespace ShaderBoxNativeBridge
{
	class INativeWindowBridge;
}

namespace ShaderBoxBridge
{
	public delegate void ManagedCallbackHandler();

	public ref class ViewportHost : HwndHost
	{
	public:
		ViewportHost(ManagedCallbackHandler^ callbackWindowCreated);
		void SetUnregisteredCallback(ManagedCallbackHandler^ callbackWindowDestroyed) { m_UnregisterCallback = callbackWindowDestroyed; }
		
		void SetModel(String^ filePath);

		void SetShaders(String^ vertex, String^ hull, String^ domain, String^ geometry, String^ pixel);
		void UpdateShaders(String^ vertex, String^ hull, String^ domain, String^ geometry, String^ pixel);
		
		void SetTopology(const int topology);
		void SetRasterizerState(const int cullMode, const int fillMode);
		void SetCameraOffset(const float offset);

		void UpdateShaderVariables(const UINT slotIndex, const int shaderType, array<Byte>^ data, const UINT length);
		void UpdateShaderImage(const UINT slotIndex, const int shaderType, String^ path);

		void SetPPShader(String^ pixel);
		void UpdatePPShader(String^ pixel);

		void UpdatePPShaderVariables(const UINT slotIndex, array<Byte>^ data, const UINT length);
		void UpdatePPShaderImage(const UINT slotIndex, String^ path);

		void RenderThumbnail(String^ path, ManagedCallbackHandler^ callbackThumbnailRendered);
		void PopCallbackRenderThumbnail();
	protected:
		// Inherited via HwndHost
		virtual HandleRef BuildWindowCore(HandleRef hwndParent) override;
		virtual void DestroyWindowCore(HandleRef hwnd) override;
		virtual IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, bool% handled) override;
	private:
		ShaderBoxNativeBridge::INativeWindowBridge* m_pWindow;

		ManagedCallbackHandler^ m_RegisterCallback;
		ManagedCallbackHandler^ m_UnregisterCallback;
		System::Collections::Generic::Queue<ManagedCallbackHandler^>^ m_RenderThumbCallbackVec;
	};


}
