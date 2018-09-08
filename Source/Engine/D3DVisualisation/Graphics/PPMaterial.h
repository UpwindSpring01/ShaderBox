#pragma once

namespace SpringWindEngine
{
	class PPMaterial
	{
	public:
		PPMaterial(const std::wstring& pixelShader);
		virtual ~PPMaterial();

		ID3D11PixelShader* GetShader() const { return m_pPixelShader; }

		virtual void BeginDraw() {};
		virtual void EndDraw() {};
	protected:
		ID3D11PixelShader* m_pPixelShader = nullptr;

		PPMaterial(const PPMaterial &obj) = delete;
		PPMaterial& operator=(const PPMaterial& obj) = delete;
	};
}