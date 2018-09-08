#include "stdafx.h"

#include "Bridge.h"
#include "NativeBridge/Interfaces/INativeBridge.h"

#include "msclr/marshal_cppstd.h"

using namespace ShaderBoxNativeBridge;
using namespace System::Runtime::InteropServices;

void ShaderBoxBridge::Bridge::InitEngine()
{
	INativeBridge* bridge = INativeBridge::CreateNativeBridge();
	bridge->InitEngine();
}

void ShaderBoxBridge::Bridge::ShutdownEngine()
{
	INativeBridge::GetNativeBridge()->ShutdownEngine();
	INativeBridge::DestroyNativeBridge();
}

void ShaderBoxBridge::Bridge::CreateThumbnailModel(String^ model, String^ saveLocation, ManagedCallbackHandler^ callback)
{
	if (m_RenderThumbCallbackVec == nullptr)
	{
		m_RenderThumbCallbackVec = gcnew System::Collections::Generic::Queue<ManagedCallbackHandler^>();
	}
	m_RenderThumbCallbackVec->Enqueue(callback);

	IntPtr cbPtr = Marshal::GetFunctionPointerForDelegate(callback);
	INativeBridge::GetNativeBridge()->CreateThumbnailModel(msclr::interop::marshal_as<std::wstring>(model),
		msclr::interop::marshal_as<std::wstring>(saveLocation), static_cast<REGISTER_WINDOW_CALLBACK>(cbPtr.ToPointer()));
}

void ShaderBoxBridge::Bridge::PopCallbackRenderThumbnail()
{
	m_RenderThumbCallbackVec->Dequeue();
}