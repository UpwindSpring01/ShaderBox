#include "stdafx.h"

#include "EffectFactory.h"
#include "ShaderStorage.h"

using namespace SpringWindEngine;

SpringWindEngine::EffectFactory::~EffectFactory()
{
	for (const std::pair<size_t, Effect*>& kvp : m_EffectPtrMap)
	{
		assert(kvp.second->Counter == 0);
		ShaderStorage::GetInstance()->UnloadEffectShaders(kvp.second);
		delete kvp.second;
	}
}

Effect* SpringWindEngine::EffectFactory::LoadEffect(const std::wstring& vertexFile, const std::wstring& pixelFile, const std::wstring& geometryFile, const std::wstring& hullFile, const std::wstring& domainFile)
{
	assert(hullFile.empty() == domainFile.empty());

	size_t hash = 0;
	hash_combine(hash, vertexFile);
	hash_combine(hash, pixelFile);
	if (!geometryFile.empty())
	{
		hash_combine(hash, geometryFile);
	}
	if (!hullFile.empty())
	{
		hash_combine(hash, hullFile);
		hash_combine(hash, domainFile);
	}

	std::unordered_map<size_t, Effect*>::const_iterator it = m_EffectPtrMap.find(hash);
	if (it != m_EffectPtrMap.cend())
	{
		return it->second;
	}

	Effect* pEffect = new Effect();

	ShaderStorage* pStorage = ShaderStorage::GetInstance();
	pEffect->pVertexShader = pStorage->GetVertexShader(vertexFile, &pEffect->pInputLayoutDesc);
	pEffect->pPixelShader = pStorage->GetPixelShader(pixelFile);
	if (!geometryFile.empty())
	{
		pEffect->pGeometryShader = pStorage->GetGeometryShader(geometryFile);
	}
	if (!hullFile.empty())
	{
		pEffect->pHullShader = pStorage->GetHullShader(hullFile);
		pEffect->pDomainShader = pStorage->GetDomainShader(hullFile);
	}

	pEffect->Hash = hash;
	m_EffectPtrMap.emplace(hash, pEffect);
	return pEffect;
}

void SpringWindEngine::EffectFactory::ForceRefreshEffect(Effect* pEffect, const std::wstring& vertexFile, const std::wstring& pixelFile, const std::wstring& geometryFile, const std::wstring& hullFile, const std::wstring& domainFile)
{
	assert(hullFile.empty() == domainFile.empty());

	// A file addition might have occured, so rehash it
	size_t newHash = 0;
	hash_combine(newHash, vertexFile);
	hash_combine(newHash, pixelFile);
	if (!geometryFile.empty())
	{
		hash_combine(newHash, geometryFile);
	}
	if (!hullFile.empty())
	{
		hash_combine(newHash, hullFile);
		hash_combine(newHash, domainFile);
	}

	if (newHash != pEffect->Hash)
	{
		auto pair = m_EffectPtrMap.extract(pEffect->Hash);
		pair.key() = newHash;
		m_EffectPtrMap.insert(std::move(pair));

		pEffect->Hash = newHash;
	}

	ShaderStorage* pStorage = ShaderStorage::GetInstance();
	pEffect->pVertexShader = pStorage->RefreshVertexShaderForced(pEffect->pVertexShader, pEffect->pInputLayoutDesc, vertexFile, &pEffect->pInputLayoutDesc);
	pEffect->pPixelShader = pStorage->RefreshPixelShaderForced(pEffect->pPixelShader, pixelFile);
	if (!geometryFile.empty())
	{
		pEffect->pGeometryShader = pStorage->RefreshGeometryShaderForced(pEffect->pGeometryShader, geometryFile);
	}
	else
	{
		pEffect->pGeometryShader = nullptr;
	}

	if (!hullFile.empty())
	{
		pEffect->pHullShader = pStorage->RefreshHullShaderForced(pEffect->pHullShader, hullFile);
		pEffect->pDomainShader = pStorage->RefreshDomainShaderForced(pEffect->pDomainShader, hullFile);
	}
	else
	{
		pEffect->pHullShader = nullptr;
		pEffect->pDomainShader = nullptr;
	}
}

void SpringWindEngine::EffectFactory::UnloadAndDestroyEffect(const Effect* pEffect)
{
	assert(pEffect->Counter == 0);

	ShaderStorage::GetInstance()->UnloadEffectShaders(pEffect);
	m_EffectPtrMap.erase(pEffect->Hash);

	delete pEffect;
}

void SpringWindEngine::EffectFactory::UnloadAndDestroyUnusedEffects()
{
	for (std::unordered_map<size_t, Effect*>::const_iterator it = m_EffectPtrMap.cbegin(); it != m_EffectPtrMap.cend();)
	{
		if (it->second->Counter == 0)
		{
			ShaderStorage::GetInstance()->UnloadEffectShaders(it->second);
			delete it->second;
			it = m_EffectPtrMap.erase(it);
		}
		else
		{
			++it;
		}
	}
}