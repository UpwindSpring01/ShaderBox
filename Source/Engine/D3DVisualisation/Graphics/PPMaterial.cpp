#include "stdafx.h"

#include "PPMaterial.h"
#include "Content/Shaders/ShaderStorage.h"

SpringWindEngine::PPMaterial::PPMaterial(const std::wstring& pixelShader)
	: m_pPixelShader(ShaderStorage::GetInstance()->GetPixelShader(pixelShader))
{
}

SpringWindEngine::PPMaterial::~PPMaterial()
{
	ShaderStorage::GetInstance()->UnloadPixelShader(m_pPixelShader);
}