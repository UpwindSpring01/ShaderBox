#include "stdafx.h"
#include "TextureDataLoader.h"
#include <algorithm>


TextureDataLoader::TextureDataLoader(void)
{
}


TextureDataLoader::~TextureDataLoader(void)
{
}

TextureData* TextureDataLoader::Load(const std::initializer_list<std::wstring>& assetFiles)
{
	ID3D11Resource* pTexture;
	ID3D11ShaderResourceView* pShaderResourceView;

	TexMetadata info;
	ScratchImage *image = new ScratchImage();

	//Find Extension
	std::wstring assetFile = *assetFiles.begin();

	std::wstring extension;
	if (assetFile.find_last_of(L".") != std::wstring::npos)
	{
		extension = assetFile.substr(assetFile.find_last_of(L".") + 1);
	}
#ifdef _DEBUG
	else
	{
		Logger::LogFormat(Logger::LogLevel::Error, L"TextureDataLoader::LoadContent() > Invalid File Extensions!\nPath: %s", assetFile.c_str());
		return nullptr;
	}
#endif // _DEBUG	

	if (lstrcmpiW(extension.c_str(), L"dds") == 0) //DDS Loader
	{
		CHECK_HR(LoadFromDDSFile(assetFile.c_str(), DDS_FLAGS_NONE, &info, *image));
	}
	else if (lstrcmpiW(extension.c_str(), L"tga") == 0) //TGA Loader
	{
		CHECK_HR(LoadFromTGAFile(assetFile.c_str(), &info, *image));
	}
	else //WIC Loader
	{
		CHECK_HR(LoadFromWICFile(assetFile.c_str(), WIC_FLAGS_NONE, &info, *image));
	}
	
	ID3D11Device* pDevice = SpringWind::GetInstance()->GetGameContext().pDevice;
	CHECK_HR(CreateTexture(pDevice, image->GetImages(), image->GetImageCount(),image->GetMetadata(), &pTexture));

	CHECK_HR(CreateShaderResourceView(pDevice, image->GetImages(), image->GetImageCount(), image->GetMetadata(), &pShaderResourceView));
	
	delete image;
	return new TextureData(pTexture, pShaderResourceView);
}