#pragma once
#include "ContentLoader.h"
#include "..\Graphics\TextureData.h"

class TextureDataLoader : public ContentLoader<TextureData>
{
public:
	TextureDataLoader(void);
	~TextureDataLoader(void);

	TextureData* Load(const std::initializer_list<std::wstring>& assetFiles) override;
	void Update(const std::initializer_list<std::wstring>& /*assetFiles*/, TextureData* /*content*/) override {};
private:
// -------------------------
// Disabling default copy constructor and default 
// assignment operator.
// -------------------------
TextureDataLoader(const TextureDataLoader& yRef);									
TextureDataLoader& operator=(const TextureDataLoader& yRef);
};

