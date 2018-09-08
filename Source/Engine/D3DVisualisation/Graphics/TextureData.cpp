#include "stdafx.h"
#include "TextureData.h"


SpringWindEngine::TextureData::TextureData(size_t width, size_t height, ID3D11ShaderResourceView* pTextureShaderResourceView)
	: m_Dimension(static_cast<float>(width), static_cast<float>(height)),
	m_pTextureShaderResourceView(pTextureShaderResourceView)
{
}


SpringWindEngine::TextureData::~TextureData()
{
	SafeRelease(m_pTextureShaderResourceView);
}
