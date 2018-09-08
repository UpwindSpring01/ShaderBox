#pragma once

#include "ContentLoader.h"
namespace SpringWindEngine
{
	class MeshLoader final : public ContentLoader<Mesh>
	{
	public:
		Unsafe_Shared_Ptr<Mesh> Load(const std::wstring& assetFile) override;

		void UnloadUnusedMeshBuffers();
	private:
		friend class ContentManager;
		MeshLoader(void) {};
		~MeshLoader(void) {};

		// -------------------------
		// Disabling default copy constructor and default 
		// assignment operator.
		// -------------------------
		MeshLoader(const MeshLoader &obj) = delete;
		MeshLoader& operator=(const MeshLoader& obj) = delete;
	};
}