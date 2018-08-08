#pragma once

class Material
{
public:
	Material();

	virtual ~Material();

	void SetShadersAndBuffers(const GameContext& gameContext, const TransformComponent* pTransComp);
	void UpdateBuffers(const UINT cbufferIndex, const int shaderType, const byte* data, const UINT length);
	void UpdateImageBuffers(const UINT cbufferIndex, const int shaderType, const std::wstring path);
	void SetEffect(Effect* pEffect);
	void BeginDraw() {};
	void EndDraw() {};
private:
	Effect* m_pEffect = nullptr;

	Material(const Material &obj) = delete;
	Material& operator=(const Material& obj) = delete;
};