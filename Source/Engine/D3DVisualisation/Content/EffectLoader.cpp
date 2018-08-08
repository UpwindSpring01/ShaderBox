//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "EffectLoader.h"
#include "../Helpers/EffectHelper.h"

#include <fstream>


byte* EffectLoader::LoadShaderFile(const std::wstring file, UINT& length)
{
	byte *pFileData = nullptr;

	// open the file
	std::ifstream fileStream(file, std::ios::in | std::ios::binary | std::ios::ate);

	// if open was successful
#ifdef _DEBUG
	if (!fileStream.is_open())
	{
		Logger::LogError(L"EffectLoader::LoadShaderFile -> File not found: " + file);
	}
#endif // _DEBUG


	// find the length of the file
	length = (int)fileStream.tellg();

	// collect the file data
	pFileData = new byte[length];
	fileStream.seekg(0, std::ios::beg);
	fileStream.read(reinterpret_cast<char*>(pFileData), length);
	fileStream.close();

	return pFileData;
}

void EffectLoader::Update(const std::initializer_list<std::wstring>& assetFiles, Effect* pEffect)
{
	pEffect->Release();
	LoadInternal(assetFiles, pEffect);
}

Effect* EffectLoader::Load(const std::initializer_list<std::wstring>& assetFiles)
{
	Effect* pEffect = new Effect();
	LoadInternal(assetFiles, pEffect);
	return pEffect;
}

void EffectLoader::LoadInternal(const std::initializer_list<std::wstring>& assetFiles, Effect* pEffect)
{
#ifdef _DEBUG
	if (assetFiles.size() != 6)
	{
		Logger::LogError(L"EffectLoader::LoadContent -> Needs 6 strings: Identifier - Vertex - Hull - Domain - Geometry - Pixel shader\nHull, Domain and Geometry can be empty");
	}
#endif // _DEBUG

	ID3D11Device* pDevice = SpringWind::GetInstance()->GetGameContext().pDevice;

	std::initializer_list<std::wstring>::const_iterator it = assetFiles.begin();
	// Skip identifier
	++it;

	// Vertex Shader
	UINT length = 0;
	byte* data = nullptr;
	std::unordered_map<std::wstring, ID3D11DeviceChild*>::const_iterator shaderMapIt;


	data = LoadShaderFile(*it, length);
	CHECK_HR(pDevice->CreateVertexShader(data, length, nullptr, &pEffect->VertexShaderPtr));
	CHECK_HR(EffectHelper::BuildInputLayout(pDevice, data, length, &pEffect->InputLayoutPtr, pEffect->m_pInputLayoutDescriptions, pEffect->m_pInputLayoutSize, pEffect->m_InputLayoutID));
	delete[] data;

	++it;
	// Hull Shader
	if (!(*it).empty())
	{
		data = LoadShaderFile(*it, length);
		CHECK_HR(pDevice->CreateHullShader(data, length, nullptr, &pEffect->HullShaderPtr));
		delete[] data;
	}

	++it;
	// Domain Shader
	if (!(*it).empty())
	{
		data = LoadShaderFile(*it, length);
		CHECK_HR(pDevice->CreateDomainShader(data, length, nullptr, &pEffect->DomainShaderPtr));
		delete[] data;
	}

	++it;
	// Geometry Shader
	if (!(*it).empty())
	{
		data = LoadShaderFile(*it, length);
		CHECK_HR(pDevice->CreateGeometryShader(data, length, nullptr, &pEffect->GeometryShaderPtr));
		delete[] data;
	}

	++it;
	// Pixel Shader
	data = LoadShaderFile(*it, length);
	CHECK_HR(pDevice->CreatePixelShader(data, length, nullptr, &pEffect->PixelShaderPtr));
	delete[] data;
}