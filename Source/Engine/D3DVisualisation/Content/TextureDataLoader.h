#pragma once
#include "ContentLoader.h"
#include "..\Graphics\TextureData.h"

namespace SpringWindEngine
{
	class TextureDataLoader : public ContentLoader<TextureData>
	{
	public:
		Unsafe_Shared_Ptr<TextureData> Load(const std::wstring& assetFile) override;
	private:
		friend class ContentManager;
		TextureDataLoader();
		~TextureDataLoader();
		// -------------------------
		// Disabling default copy constructor and default 
		// assignment operator.
		// -------------------------
		TextureDataLoader(const TextureDataLoader& yRef) = delete;
		TextureDataLoader& operator=(const TextureDataLoader& yRef) = delete;
	};
}