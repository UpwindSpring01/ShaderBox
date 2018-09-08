#pragma once
namespace SpringWindEngine
{
	class TextureData
	{
	public:
		TextureData(size_t width, size_t height, ID3D11ShaderResourceView* pTextureShaderResourceView);
		~TextureData(void);

		ID3D11ShaderResourceView* GetShaderResourceView() const { return m_pTextureShaderResourceView; }
		XMFLOAT2 GetDimension() const { return m_Dimension; }

	private:

		ID3D11ShaderResourceView *m_pTextureShaderResourceView;
		XMFLOAT2 m_Dimension;

	private:
		// -------------------------
		// Disabling default copy constructor and default 
		// assignment operator.
		// -------------------------
		TextureData(const TextureData& t) = delete;
		TextureData& operator=(const TextureData& t) = delete;
	};
}