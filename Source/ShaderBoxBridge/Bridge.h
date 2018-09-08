#pragma once

using namespace System;

namespace ShaderBoxBridge
{
	public delegate void ManagedCallbackHandler();

	public ref class Bridge abstract sealed
	{
	public:
		static void InitEngine();
		static void ShutdownEngine();

		static void CreateThumbnailModel(String^ model, String^ saveLocation, ManagedCallbackHandler^ callback);
		static void PopCallbackRenderThumbnail();
	private:
		static System::Collections::Generic::Queue<ManagedCallbackHandler^>^ m_RenderThumbCallbackVec = nullptr;
	};
}