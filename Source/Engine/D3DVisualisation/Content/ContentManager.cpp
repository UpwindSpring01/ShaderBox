#include "stdafx.h"

#include "ContentManager.h"

std::vector<BaseLoader*> ContentManager::m_Loaders = std::vector<BaseLoader*>();

void ContentManager::AddLoader(BaseLoader* loader)
{
	m_Loaders.push_back(loader);
}

void ContentManager::Release()
{
	for (BaseLoader* ldr : m_Loaders)
	{
		delete ldr;
	}

	m_Loaders.clear();
}