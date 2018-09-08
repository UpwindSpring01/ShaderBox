//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "MeshLoader.h"

#include <fstream>

using namespace SpringWindEngine;

Unsafe_Shared_Ptr<Mesh> SpringWindEngine::MeshLoader::Load(const std::wstring& assetFile)
{
	WaveFrontReader<unsigned int> reader;
	reader.Load(assetFile.c_str());
	
	UINT elements = ILSemantic::POSITION;
	if (reader.hasNormals)
	{
		elements |= ILSemantic::NORMAL;
	}
	if (reader.hasTexcoords)
	{
		elements |= ILSemantic::TEXCOORD;
	}
	
	return Unsafe_Make_Shared<Mesh>(reader.vertices, reader.indices, elements);
}

void SpringWindEngine::MeshLoader::UnloadUnusedMeshBuffers()
{
	for (const std::pair<std::wstring, Unsafe_Shared_Ptr<Mesh>>& pair : m_contentReferencesPtrMap)
	{
		pair.second->UnloadUnusedBuffers();
	}
}
