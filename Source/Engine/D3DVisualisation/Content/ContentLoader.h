#pragma once

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
	ContentLoader() {};
	virtual ~ContentLoader() 
	{
		for (std::pair<std::wstring, T*> kvp : m_contentReferences)
		{
			delete kvp.second;
		}
		m_contentReferences.clear();
	};

	T* LoadContent(const std::initializer_list<std::wstring>& assetFiles)
	{
		std::wstring storageString = *assetFiles.begin();
		for (std::pair<std::wstring, T*> kvp : m_contentReferences)
		{
			if (kvp.first.compare(storageString) == 0)
			{
				return kvp.second;
			}
		}
		T* content = Load(assetFiles);
		m_contentReferences.insert(std::pair<std::wstring, T*>(storageString, content));

		return content;
	}

	bool UpdateContent(const std::initializer_list<std::wstring>& assetFiles)
	{
		std::wstring storageString = *assetFiles.begin();
		for (std::pair<std::wstring, T*> kvp : m_contentReferences)
		{
			if (kvp.first.compare(storageString) == 0)
			{
				Update(assetFiles, kvp.second);
				return true;
			}
		}
		return false;
	}
protected:
	virtual T* Load(const std::initializer_list<std::wstring>& assetFiles) = 0;
	virtual void Update(const std::initializer_list<std::wstring>& assetFiles, T* content) =0;
private:
	static std::unordered_map<std::wstring, T*> m_contentReferences;

	ContentLoader(const ContentLoader& t) = delete;
	ContentLoader& operator=(const ContentLoader& t) = delete;
};

template<typename T>
std::unordered_map<std::wstring, T*> ContentLoader<T>::m_contentReferences = std::unordered_map<std::wstring, T*>();