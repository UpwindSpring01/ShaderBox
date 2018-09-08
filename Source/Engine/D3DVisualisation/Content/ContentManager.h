#pragma once

#include "ContentLoader.h"

namespace SpringWindEngine
{
	class MeshLoader;

	class ContentManager final
	{
	public:
		template<class T>
		static Unsafe_Shared_Ptr<T> Load(const std::wstring& assetFiles)
		{
			for (BaseLoader* loader : m_Loaders)
			{
				if (dynamic_cast<ContentLoader<T>*>(loader) != nullptr)
				{
					return (static_cast<ContentLoader<T>*>(loader))->LoadContent(assetFiles);
				}
			}
			return nullptr;
		}

		template<class T>
		static void UnloadUnused()
		{
			for (BaseLoader* loader : m_Loaders)
			{
				if (dynamic_cast<ContentLoader<T>*>(loader) != nullptr)
				{
					return (static_cast<ContentLoader<T>*>(loader))->UnloadUnused();
				}
			}
			return false;
		}

		static void Initialize();
		static void Shutdown();

		static MeshLoader* GetMeshLoader();
	private:
		ContentManager() {};
		~ContentManager(void) {};

		static std::vector<BaseLoader*> m_Loaders;

		ContentManager(const ContentManager& t) = delete;
		ContentManager& operator=(const ContentManager& t) = delete;
	};
}