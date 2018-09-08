#pragma once
#include "Graphics/Material.h"

namespace ShaderBoxNativeBridge
{
	class ShaderBoxMaterial final : public SpringWindEngine::Material
	{
	public:
		ShaderBoxMaterial(SpringWindEngine::Effect* pEffect);
		virtual ~ShaderBoxMaterial();
		virtual void BeginDraw() override;
		virtual void EndDraw() override;
		
		void SetBuffer(const UINT slotIndex, const int shaderType, const std::byte* data, const UINT length);
		void SetImageBuffer(const UINT slotIndex, const int shaderType, const std::wstring& path);
		void SetRasterizerState(const int cullMode, const int fillMode);
	private:
		std::unordered_map<UINT, ID3D11Buffer*> m_VertexBufferPtrMap;
		std::unordered_map<UINT, ID3D11Buffer*> m_HullBufferPtrMap;
		std::unordered_map<UINT, ID3D11Buffer*> m_DomainBufferPtrMap;
		std::unordered_map<UINT, ID3D11Buffer*> m_GeometryBufferPtrMap;
		std::unordered_map<UINT, ID3D11Buffer*> m_PixelBufferPtrMap;
		
		std::unordered_map<UINT, SpringWindEngine::Unsafe_Shared_Ptr<SpringWindEngine::TextureData>> m_VertexTextureDataPtrMap;
		std::unordered_map<UINT, SpringWindEngine::Unsafe_Shared_Ptr<SpringWindEngine::TextureData>> m_HullTextureDataPtrMap;
		std::unordered_map<UINT, SpringWindEngine::Unsafe_Shared_Ptr<SpringWindEngine::TextureData>> m_DomainTextureDataPtrMap;
		std::unordered_map<UINT, SpringWindEngine::Unsafe_Shared_Ptr<SpringWindEngine::TextureData>> m_GeometryTextureDataPtrMap;
		std::unordered_map<UINT, SpringWindEngine::Unsafe_Shared_Ptr<SpringWindEngine::TextureData>> m_PixelTextureDataPtrMap;

		ID3D11RasterizerState* m_pRasterizerState = nullptr;
	};
}