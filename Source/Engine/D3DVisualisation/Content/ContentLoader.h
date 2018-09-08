#pragma once

namespace SpringWindEngine
{
	class BaseLoader
	{
	public:
		BaseLoader() {};
		virtual ~BaseLoader() {};

		BaseLoader(const BaseLoader& t) = delete;
		BaseLoader& operator=(const BaseLoader& t) = delete;
	};

	template<class T>
	class ContentLoader : public BaseLoader
	{
	public:
		using StorageMap = std::unordered_map<std::wstring, Unsafe_Shared_Ptr<T>>;
		ContentLoader() {};
		virtual ~ContentLoader()
		{
			m_contentReferencesPtrMap.clear();
		};

		Unsafe_Shared_Ptr<T> LoadContent(const std::wstring& assetFile)
		{
			for (std::pair<std::wstring, Unsafe_Shared_Ptr<T>> kvp : m_contentReferencesPtrMap)
			{
				if (kvp.first.compare(assetFile) == 0)
				{
					return kvp.second;
				}
			}
			Unsafe_Shared_Ptr<T> content = Load(assetFile);
			m_contentReferencesPtrMap.emplace(assetFile, content);

			return content;
		}

		void UnloadUnused()
		{
			for (typename StorageMap::const_iterator it = m_contentReferencesPtrMap.cbegin(); it != m_contentReferencesPtrMap.cend();)
			{
				if (it->second.UseCount() == 1)
				{
					it = m_contentReferencesPtrMap.erase(it);
				}
				else
				{
					++it;
				}
			}
		}
	protected:
		virtual Unsafe_Shared_Ptr<T> Load(const std::wstring& assetFile) = 0;

		StorageMap m_contentReferencesPtrMap;
	private:
		ContentLoader(const ContentLoader& t) = delete;
		ContentLoader& operator=(const ContentLoader& t) = delete;
	};
}