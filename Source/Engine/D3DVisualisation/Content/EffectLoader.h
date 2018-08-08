#pragma once

#include "ContentLoader.h"

class EffectLoader final : public ContentLoader<Effect>
{
public:
	Effect* Load(const std::initializer_list<std::wstring>& assetFiles) override;
	void Update(const std::initializer_list<std::wstring>& assetFiles, Effect* pEffect) override;
private:
	friend class SpringWind;

	void LoadInternal(const std::initializer_list<std::wstring>& assetFiles, Effect* pEffect);
	EffectLoader(void) {};
	~EffectLoader(void) {};

	//std::unordered_map<std::wstring, ID3D11DeviceChild*> m_shadersMap;
	byte* LoadShaderFile(const std::wstring File, UINT& length);
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	EffectLoader(const EffectLoader &obj) = delete;
	EffectLoader& operator=(const EffectLoader& obj) = delete;
};

