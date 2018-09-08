#pragma once

#include "Helpers/Singleton.h"

namespace SpringWindEngine
{
	class ShaderStorage final : public Singleton<ShaderStorage>
	{
	public:
		ID3D11VertexShader* GetVertexShader(const std::wstring& assetFile, InputLayoutDesc** inputLayoutDesc);
		ID3D11PixelShader* GetPixelShader(const std::wstring& assetFile);

		ID3D11PixelShader* RefreshPixelShaderForced(const ID3D11PixelShader* pOldShader, const std::wstring& assetFile);

		void UnloadVertexShader(const ID3D11VertexShader* pVertexShader, const InputLayoutDesc* pInputLayoutDesc);
		void UnloadPixelShader(const ID3D11PixelShader* pPixelShader);
		void DestroyUnusedShaders();
	private:
		friend class EffectFactory;
		ID3D11HullShader* GetHullShader(const std::wstring& assetFile);
		ID3D11DomainShader* GetDomainShader(const std::wstring& assetFile);
		ID3D11GeometryShader* GetGeometryShader(const std::wstring& assetFile);
		

		ID3D11VertexShader* RefreshVertexShaderForced(const ID3D11VertexShader* pOldShader, const InputLayoutDesc* pOldInputLayoutDesc, const std::wstring& assetFile, InputLayoutDesc** inputLayoutDesc);
		ID3D11HullShader* RefreshHullShaderForced(const ID3D11HullShader* pOldShader, const std::wstring& assetFile);
		ID3D11DomainShader* RefreshDomainShaderForced(const ID3D11DomainShader* pOldShader, const std::wstring& assetFile);
		ID3D11GeometryShader* RefreshGeometryShaderForced(const ID3D11GeometryShader* pOldShader, const std::wstring& assetFile);

		void UnloadEffectShaders(const Effect* pEffect);

		byte* LoadShaderFile(const std::wstring file, UINT& length);
		InputLayoutDesc* BuildInputLayout(ID3D11Device* pDevice, const byte* data, const UINT length);

		friend class Singleton<ShaderStorage>;
		ShaderStorage() {};
		~ShaderStorage();

		std::unordered_map<size_t, std::pair<InputLayoutDesc*, UINT>> m_InputLayoutPtrMap;
		std::unordered_map<size_t, std::pair<ID3D11VertexShader*, UINT>> m_VertexShaderPtrMap;
		std::unordered_map<size_t, std::pair<ID3D11HullShader*, UINT>> m_HullShaderPtrMap;
		std::unordered_map<size_t, std::pair<ID3D11DomainShader*, UINT>> m_DomainShaderPtrMap;
		std::unordered_map<size_t, std::pair<ID3D11GeometryShader*, UINT>> m_GeometryShaderPtrMap;
		std::unordered_map<size_t, std::pair<ID3D11PixelShader*, UINT>> m_PixelShaderPtrMap;

		ShaderStorage(const ShaderStorage &obj) = delete;
		ShaderStorage& operator=(const ShaderStorage& obj) = delete;
	};
}