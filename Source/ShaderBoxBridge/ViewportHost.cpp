#include "stdafx.h"

#include <functional>

#include "ViewportHost.h"
#include "NativeBridge/Interfaces/INativeBridge.h"
#include "NativeBridge/Interfaces/INativeWindowBridge.h"

#include "msclr/marshal_cppstd.h"

using namespace ShaderBoxNativeBridge;

ShaderBoxBridge::ViewportHost::ViewportHost(ManagedCallbackHandler^ callbackWindowCreated)
{
	m_RegisterCallback = callbackWindowCreated;
	WindowInteropHelper^ helper = gcnew WindowInteropHelper(System::Windows::Application::Current->MainWindow);
	IntPtr cbPtr = Marshal::GetFunctionPointerForDelegate(callbackWindowCreated);
	m_pWindow = INativeBridge::GetNativeBridge()->RegisterWindow(static_cast<HWND>(helper->Handle.ToPointer()), static_cast<REGISTER_WINDOW_CALLBACK>(cbPtr.ToPointer()));
}

void ShaderBoxBridge::ViewportHost::SetModel(System::String^ filePath)
{
	m_pWindow->SetModel(msclr::interop::marshal_as<std::wstring>(filePath));
}

void ShaderBoxBridge::ViewportHost::SetShaders(String^ vertex, String^ hull, String^ domain, String^ geometry, String^ pixel)
{
	m_pWindow->SetShaders(msclr::interop::marshal_as<std::wstring>(vertex),
		msclr::interop::marshal_as<std::wstring>(hull), msclr::interop::marshal_as<std::wstring>(domain),
		msclr::interop::marshal_as<std::wstring>(geometry), msclr::interop::marshal_as<std::wstring>(pixel));
}

void ShaderBoxBridge::ViewportHost::UpdateShaders(String^ vertex, String^ hull, String^ domain, String^ geometry, String^ pixel)
{
	m_pWindow->UpdateShaders(msclr::interop::marshal_as<std::wstring>(vertex),
		msclr::interop::marshal_as<std::wstring>(hull), msclr::interop::marshal_as<std::wstring>(domain),
		msclr::interop::marshal_as<std::wstring>(geometry), msclr::interop::marshal_as<std::wstring>(pixel));
}

void ShaderBoxBridge::ViewportHost::UpdateShaderVariables(const UINT slotIndex, const int shaderType, array<Byte>^ data, const UINT length)
{
	pin_ptr<Byte> pp = &data[0];
	m_pWindow->UpdateBuffer(slotIndex, shaderType, reinterpret_cast<std::byte*>(pp), length);
}

void ShaderBoxBridge::ViewportHost::UpdateShaderImage(const UINT slotIndex, const int shaderType, String^ path)
{
	m_pWindow->UpdateImageBuffer(slotIndex, shaderType, msclr::interop::marshal_as<std::wstring>(path));
}

void ShaderBoxBridge::ViewportHost::SetPPShader(String^ pixel)
{
	m_pWindow->SetPPShader(msclr::interop::marshal_as<std::wstring>(pixel));
}

void ShaderBoxBridge::ViewportHost::UpdatePPShader(String^ pixel)
{
	m_pWindow->UpdatePPShader(msclr::interop::marshal_as<std::wstring>(pixel));
}

void ShaderBoxBridge::ViewportHost::UpdatePPShaderVariables(const UINT slotIndex, array<Byte>^ data, const UINT length)
{
	pin_ptr<Byte> pp = &data[0];
	m_pWindow->UpdatePPBuffer(slotIndex, reinterpret_cast<std::byte*>(pp), length);
}

void ShaderBoxBridge::ViewportHost::UpdatePPShaderImage(const UINT slotIndex, String^ path)
{
	m_pWindow->UpdatePPImageBuffer(slotIndex, msclr::interop::marshal_as<std::wstring>(path));
}

void ShaderBoxBridge::ViewportHost::SetTopology(const int topology)
{
	m_pWindow->SetTopology(topology);
}

void ShaderBoxBridge::ViewportHost::SetRasterizerState(const int cullMode, const int fillMode)
{
	m_pWindow->SetRasterizerState(cullMode, fillMode);
}

void ShaderBoxBridge::ViewportHost::SetCameraOffset(const float offset)
{
	m_pWindow->SetCameraOffset(offset);
}

void ShaderBoxBridge::ViewportHost::RenderThumbnail(String^ path, ManagedCallbackHandler^ callbackThumbnailRendered)
{
	if (m_RenderThumbCallbackVec == nullptr)
	{
		m_RenderThumbCallbackVec = gcnew System::Collections::Generic::Queue<ManagedCallbackHandler^>();
	}
	m_RenderThumbCallbackVec->Enqueue(callbackThumbnailRendered);

	IntPtr cbPtr = Marshal::GetFunctionPointerForDelegate(callbackThumbnailRendered);
	m_pWindow->RenderThumbnail(msclr::interop::marshal_as<std::wstring>(path), static_cast<REGISTER_WINDOW_CALLBACK>(cbPtr.ToPointer()));
}

void ShaderBoxBridge::ViewportHost::PopCallbackRenderThumbnail()
{
	m_RenderThumbCallbackVec->Dequeue();
}

HandleRef ShaderBoxBridge::ViewportHost::BuildWindowCore(HandleRef hwndParent)
{
	m_RegisterCallback = nullptr;
	return HandleRef(this, IntPtr(m_pWindow->GetWindowHandle()));
}

void ShaderBoxBridge::ViewportHost::DestroyWindowCore(HandleRef /*hwnd*/)
{
	WindowInteropHelper^ helper = gcnew WindowInteropHelper(System::Windows::Application::Current->MainWindow);
	IntPtr cbPtr = Marshal::GetFunctionPointerForDelegate(m_UnregisterCallback);
	INativeBridge::GetNativeBridge()->UnregisterWindow(m_pWindow, static_cast<REGISTER_WINDOW_CALLBACK>(cbPtr.ToPointer()));
}

IntPtr ShaderBoxBridge::ViewportHost::WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, bool% handled)
{
	bool nativeHandled = false;
	LRESULT result = INativeBridge::GetNativeBridge()->WindowProcedure(static_cast<HWND>(hwnd.ToPointer()), static_cast<UINT>(msg), static_cast<WPARAM>(wParam.ToInt64()), static_cast<LPARAM>(lParam.ToInt64()), nativeHandled);

	if (nativeHandled)
	{
		handled = true;
		return IntPtr(result);
	}

	handled = true;
	return IntPtr::Zero;
}