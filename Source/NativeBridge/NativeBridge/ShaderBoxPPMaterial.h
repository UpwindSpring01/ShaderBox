#pragma once
#include "Graphics/PPMaterial.h"

namespace ShaderBoxNativeBridge
{
	class ShaderBoxPPMaterial final : public SpringWindEngine::PPMaterial
	{
	public:
		ShaderBoxPPMaterial(const std::wstring& pixelShader);
		virtual ~ShaderBoxPPMaterial();
		virtual void BeginDraw() override;
		virtual void EndDraw() override;

		void SetShader(ID3D11PixelShader* pShader) { m_pPixelShader = pShader; }
		void SetBuffer(const UINT slotIndex, const std::byte* data, const UINT length);
		void SetImageBuffer(const UINT slotIndex, const std::wstring& path);

		static const std::vector<ShaderBoxPPMaterial*>& GetMaterials() { return s_ShaderBoxMatsPtrVec; }
	private:
		std::unordered_map<UINT, ID3D11Buffer*> m_PixelBufferPtrMap;

		std::unordered_map<UINT, SpringWindEngine::Unsafe_Shared_Ptr<SpringWindEngine::TextureData>> m_PixelTextureDataPtrMap;

		ID3D11RasterizerState* m_pRasterizerState = nullptr;

		static std::vector<ShaderBoxPPMaterial*> s_ShaderBoxMatsPtrVec;
	};
}