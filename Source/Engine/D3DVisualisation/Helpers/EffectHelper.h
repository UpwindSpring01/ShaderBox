#pragma once

class EffectHelper final
{
public:
	static HRESULT BuildInputLayout(ID3D11Device* pDevice, const byte * data, const UINT length, ID3D11InputLayout **pInputLayout,
		std::vector<ILDescription>& inputLayoutDescriptions, UINT& inputLayoutSize, UINT& inputLayoutID);
private:
private:
	EffectHelper() {};
	~EffectHelper() {};
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	EffectHelper(const EffectHelper &obj) = delete;
	EffectHelper& operator=(const EffectHelper& obj) = delete;
};


