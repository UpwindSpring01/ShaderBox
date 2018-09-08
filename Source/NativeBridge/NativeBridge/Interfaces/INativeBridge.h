#pragma once

typedef void(__stdcall *REGISTER_WINDOW_CALLBACK)();

namespace ShaderBoxNativeBridge
{
	class INativeWindowBridge;

	class __declspec(dllexport)	INativeBridge abstract
	{
	public:
		virtual ~INativeBridge() {};

		virtual void InitEngine() = 0;
		virtual void ShutdownEngine() = 0;

		virtual INativeWindowBridge* RegisterWindow(HWND parentHandle, REGISTER_WINDOW_CALLBACK callback) = 0;
		virtual void UnregisterWindow(INativeWindowBridge* pWindowBridge, REGISTER_WINDOW_CALLBACK callback) = 0;
		
		static INativeBridge* CreateNativeBridge();
		static void DestroyNativeBridge();
		static INativeBridge* GetNativeBridge();

		virtual LRESULT WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool& handled) = 0;

		virtual void CreateThumbnailModel(const std::wstring& modelPath, const std::wstring& saveLocation, REGISTER_WINDOW_CALLBACK callback) = 0;
	private:
		static INativeBridge* m_pNativeBridge;
	};
}