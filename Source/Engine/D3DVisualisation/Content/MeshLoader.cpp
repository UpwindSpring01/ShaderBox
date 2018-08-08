//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "MeshLoader.h"

#include <fstream>

Mesh* MeshLoader::Load(const std::initializer_list<std::wstring>& assetFiles)
{
#ifdef _DEBUG
	if (assetFiles.size() > 1)
	{
		Logger::LogError(L"MeshLoader::Load -> MeshLoader only support one mesh at a time");
	}
	else if(assetFiles.size() == 0)
	{
		Logger::LogError(L"MeshLoader::Load -> No file provided");
	}
#endif // _DEBUG
	WaveFrontReader<unsigned int> reader;
	reader.Load(assetFiles.begin()->c_str());
	
	UINT elements = ILSemantic::POSITION;
	if (reader.hasNormals)
	{
		elements |= ILSemantic::NORMAL;
	}
	if (reader.hasTexcoords)
	{
		elements |= ILSemantic::TEXCOORD;
	}
	
	return new Mesh(reader.vertices, reader.indices, elements);
}

void MeshLoader::Update(const std::initializer_list<std::wstring>& assetFiles, Mesh* content)
{
#ifdef _DEBUG
	if (assetFiles.size() > 1)
	{
		Logger::LogError(L"MeshLoader::Update -> MeshLoader only support one mesh at a time");
	}
	else if (assetFiles.size() == 0)
	{
		Logger::LogError(L"MeshLoader::Update -> No file provided");
	}
#endif // _DEBUG

	WaveFrontReader<unsigned int> reader;
	reader.Load(assetFiles.begin()->c_str());
	UINT elements = ILSemantic::POSITION;
	if (reader.hasNormals)
	{
		elements |= ILSemantic::NORMAL;
	}
	if (reader.hasTexcoords)
	{
		elements |= ILSemantic::TEXCOORD;
	}
	content->SwapData(reader.vertices, reader.indices, elements);
}