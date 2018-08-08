#pragma once

#include "ContentLoader.h"

class ContentManager final
{
public:
	static void AddLoader(BaseLoader* loader);

	template<class T>
	static T* Load(const std::initializer_list<std::wstring>& assetFiles)
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
	static bool Update(const std::initializer_list<std::wstring>& assetFiles)
	{
		for (BaseLoader* loader : m_Loaders)
		{
			if (dynamic_cast<ContentLoader<T>*>(loader) != nullptr)
			{
				return (static_cast<ContentLoader<T>*>(loader))->UpdateContent(assetFiles);
			}
		}
		return false;
	}

	static void Release();
private:
	ContentManager() {};
	~ContentManager(void) {};

	static std::vector<BaseLoader*> m_Loaders;

	ContentManager(const ContentManager& t) = delete;
	ContentManager& operator=(const ContentManager& t) = delete;
};


