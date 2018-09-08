#pragma once

#include "Helpers/Singleton.h"

namespace SpringWindEngine
{
	class EffectFactory final : public Singleton<EffectFactory>
	{
	public:
		Effect* LoadEffect(const std::wstring& vertexFile, const std::wstring& pixelFile, const std::wstring& geometryFile = L"", const std::wstring& hullFile = L"", const std::wstring& domainFile = L"");

		/*
		* Internal use only, this will not update the material and mesh buffers dependent on the refreshed effect
		*/
		void ForceRefreshEffect(Effect* pEffect, const std::wstring& vertexFile, const std::wstring& pixelFile, const std::wstring& geometryFile = L"", const std::wstring& hullFile = L"", const std::wstring& domainFile = L"");

		void UnloadAndDestroyEffect(const Effect* pEffect);
		void UnloadAndDestroyUnusedEffects();

		const std::unordered_map<size_t, Effect*>& GetEffects() const { return m_EffectPtrMap; }
	private:
		friend class Singleton<EffectFactory>;
		friend struct Effect;
		EffectFactory() {};
		~EffectFactory();

		std::unordered_map<size_t, Effect*> m_EffectPtrMap;

		EffectFactory(const EffectFactory &obj) = delete;
		EffectFactory& operator=(const EffectFactory& obj) = delete;
	};
}