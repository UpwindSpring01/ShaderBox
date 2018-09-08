#include "stdafx.h"
#include "TextureDataLoader.h"
#include <algorithm>

using namespace SpringWindEngine;

SpringWindEngine::TextureDataLoader::TextureDataLoader()
{
}


SpringWindEngine::TextureDataLoader::~TextureDataLoader()
{
}

Unsafe_Shared_Ptr<TextureData> SpringWindEngine::TextureDataLoader::Load(const std::wstring& assetFile)
{
	//Find Extension
#ifdef _DEBUG
	std::wstring extension;
	if (assetFile.find_last_of(L".") != std::wstring::npos)
	{
		extension = assetFile.substr(assetFile.find_last_of(L".") + 1);
	}

	else
	{
		Logger::LogFormat(Logger::LogLevel::Error, L"TextureDataLoader::LoadContent() > Invalid File Extensions!\nPath: %s", assetFile.c_str());
		return nullptr;
	}
#else
	std::wstring extension = assetFile.substr(assetFile.find_last_of(L".") + 1);
#endif // _DEBUG	

	TexMetadata metadata;
	ScratchImage image;
	if (lstrcmpiW(extension.c_str(), L"dds") == 0) //DDS Loader
	{
		CHECK_HR(LoadFromDDSFile(assetFile.c_str(), DDS_FLAGS_NONE, &metadata, image));
	}
	else if (lstrcmpiW(extension.c_str(), L"tga") == 0) //TGA Loader
	{
		CHECK_HR(LoadFromTGAFile(assetFile.c_str(), &metadata, image));
	}
	else //WIC Loader
	{
		CHECK_HR(LoadFromWICFile(assetFile.c_str(), WIC_FLAGS_NONE, &metadata, image));
	}
	
	ID3D11ShaderResourceView* pShaderResourceView;
	CHECK_HR(CreateShaderResourceViewEx(SpringWind::GetInstance()->GetEngineContext().pDevice, image.GetImages(), image.GetImageCount(), metadata,
		D3D11_USAGE::D3D11_USAGE_IMMUTABLE, D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE, 0, 0, false, &pShaderResourceView));

	return Unsafe_Make_Shared<TextureData>(metadata.width, metadata.height, pShaderResourceView);
}