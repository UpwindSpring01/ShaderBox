#include "stdafx.h"

#include "ContentManager.h"

#include "MeshLoader.h"
#include "TextureDataLoader.h"

using namespace SpringWindEngine;

std::vector<BaseLoader*> SpringWindEngine::ContentManager::m_Loaders = std::vector<BaseLoader*>();

void SpringWindEngine::ContentManager::Initialize()
{
	m_Loaders.push_back(new MeshLoader());
	m_Loaders.push_back(new TextureDataLoader());
}

void SpringWindEngine::ContentManager::Shutdown()
{
	for (BaseLoader* ldr : m_Loaders)
	{
		delete ldr;
	}

	m_Loaders.clear();
}

SpringWindEngine::MeshLoader* SpringWindEngine::ContentManager::GetMeshLoader()
{
	return static_cast<MeshLoader*>(m_Loaders[0]);
}