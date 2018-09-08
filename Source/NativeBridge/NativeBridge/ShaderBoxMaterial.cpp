#include "stdafx.h"
#include "ShaderBoxMaterial.h"

using namespace SpringWindEngine;


ShaderBoxNativeBridge::ShaderBoxMaterial::ShaderBoxMaterial(Effect* pEffect) : Material(pEffect)
{
}

ShaderBoxNativeBridge::ShaderBoxMaterial::~ShaderBoxMaterial()
{
	for (const std::pair<UINT, ID3D11Buffer*>& kvp : m_VertexBufferPtrMap)
	{
		kvp.second->Release();
	}

	for (const std::pair<UINT, ID3D11Buffer*>& kvp : m_HullBufferPtrMap)
	{
		kvp.second->Release();
	}

	for (const std::pair<UINT, ID3D11Buffer*>& kvp : m_DomainBufferPtrMap)
	{
		kvp.second->Release();
	}

	for (const std::pair<UINT, ID3D11Buffer*>& kvp : m_GeometryBufferPtrMap)
	{
		kvp.second->Release();
	}

	for (const std::pair<UINT, ID3D11Buffer*>& kvp : m_PixelBufferPtrMap)
	{
		kvp.second->Release();
	}

	if (m_pRasterizerState != nullptr)
	{
		m_pRasterizerState->Release();
		m_pRasterizerState = nullptr;
	}
}

void ShaderBoxNativeBridge::ShaderBoxMaterial::BeginDraw()
{
	ID3D11DeviceContext* pDeviceContext = SpringWind::GetInstance()->GetEngineContext().pDeviceContext;

	// Set one by one, because unordered and might be gaps in the slots indices.
	for (const std::pair<UINT, ID3D11Buffer*>& kvp : m_VertexBufferPtrMap)
	{
		pDeviceContext->VSSetConstantBuffers(kvp.first, 1, &kvp.second);
	}

	for (const std::pair<UINT, ID3D11Buffer*>& kvp : m_HullBufferPtrMap)
	{
		pDeviceContext->HSSetConstantBuffers(kvp.first, 1, &kvp.second);
	}

	for (const std::pair<UINT, ID3D11Buffer*>& kvp : m_DomainBufferPtrMap)
	{
		pDeviceContext->DSSetConstantBuffers(kvp.first, 1, &kvp.second);
	}

	for (const std::pair<UINT, ID3D11Buffer*>& kvp : m_GeometryBufferPtrMap)
	{
		pDeviceContext->GSSetConstantBuffers(kvp.first, 1, &kvp.second);
	}

	for (const std::pair<UINT, ID3D11Buffer*>& kvp : m_PixelBufferPtrMap)
	{
		pDeviceContext->PSSetConstantBuffers(kvp.first, 1, &kvp.second);
	}

	for (const std::pair<UINT, Unsafe_Shared_Ptr<TextureData>>& kvp : m_VertexTextureDataPtrMap)
	{
		ID3D11ShaderResourceView* pSRV[] = { nullptr };
		if (kvp.second != nullptr)
		{
			pSRV[0] = kvp.second->GetShaderResourceView();
		}
		pDeviceContext->VSSetShaderResources(kvp.first, 1, pSRV);
	}

	for (const std::pair<UINT, Unsafe_Shared_Ptr<TextureData>>& kvp : m_HullTextureDataPtrMap)
	{
		ID3D11ShaderResourceView* pSRV[] = { nullptr };
		if (kvp.second != nullptr)
		{
			pSRV[0] = kvp.second->GetShaderResourceView();
		}
		pDeviceContext->HSSetShaderResources(kvp.first, 1, pSRV);
	}

	for (const std::pair<UINT, Unsafe_Shared_Ptr<TextureData>>& kvp : m_DomainTextureDataPtrMap)
	{
		ID3D11ShaderResourceView* pSRV[] = { nullptr };
		if (kvp.second != nullptr)
		{
			pSRV[0] = kvp.second->GetShaderResourceView();
		}
		pDeviceContext->DSSetShaderResources(kvp.first, 1, pSRV);
	}

	for (const std::pair<UINT, Unsafe_Shared_Ptr<TextureData>>& kvp : m_GeometryTextureDataPtrMap)
	{
		ID3D11ShaderResourceView* pSRV[] = { nullptr };
		if (kvp.second != nullptr)
		{
			pSRV[0] = kvp.second->GetShaderResourceView();
		}
		pDeviceContext->GSSetShaderResources(kvp.first, 1, pSRV);
	}

	for (const std::pair<UINT, Unsafe_Shared_Ptr<TextureData>>& kvp : m_PixelTextureDataPtrMap)
	{
		ID3D11ShaderResourceView* pSRV[] = { nullptr };
		if (kvp.second != nullptr)
		{
			pSRV[0] = kvp.second->GetShaderResourceView();
		}
		pDeviceContext->PSSetShaderResources(kvp.first, 1, pSRV);
	}

	if (m_pRasterizerState != nullptr)
	{
		pDeviceContext->RSSetState(m_pRasterizerState);
	}
}

void ShaderBoxNativeBridge::ShaderBoxMaterial::EndDraw()
{
	if (m_pRasterizerState != nullptr)
	{
		SpringWind::GetInstance()->GetEngineContext().pDeviceContext->RSSetState(nullptr);
	}
}

void ShaderBoxNativeBridge::ShaderBoxMaterial::SetBuffer(const UINT slotIndex, const int shaderType, const std::byte* data, const UINT length)
{
	std::unordered_map<UINT, ID3D11Buffer*>* pBufferPtrMap = nullptr;
	switch (shaderType)
	{
	case 0:
		pBufferPtrMap = &m_VertexBufferPtrMap;
		break;
	case 1:
		pBufferPtrMap = &m_HullBufferPtrMap;
		break;
	case 2:
		pBufferPtrMap = &m_DomainBufferPtrMap;
		break;
	case 3:
		pBufferPtrMap = &m_GeometryBufferPtrMap;
		break;
	case 4:
		pBufferPtrMap = &m_PixelBufferPtrMap;
		break;
	}

	std::unordered_map<UINT, ID3D11Buffer*>::const_iterator it = pBufferPtrMap->find(slotIndex);
	if (it != pBufferPtrMap->cend())
	{
		// Update existing
		ID3D11DeviceContext* pDeviceContext = SpringWind::GetInstance()->GetEngineContext().pDeviceContext;
		D3D11_MAPPED_SUBRESOURCE resource;
		pDeviceContext->Map(it->second, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		memcpy(resource.pData, data, length);
		pDeviceContext->Unmap(it->second, 0);
		return;
	}

	// Create new
	ID3D11Device* pDevice = SpringWind::GetInstance()->GetEngineContext().pDevice;

	D3D11_BUFFER_DESC constantBufferDesc = {};
	constantBufferDesc.StructureByteStride = 0;
	constantBufferDesc.MiscFlags = 0;

	constantBufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.ByteWidth = length;

	ID3D11Buffer* buffer;
	D3D11_SUBRESOURCE_DATA subresourceData;
	subresourceData.pSysMem = data;

	CHECK_HR(pDevice->CreateBuffer(
		&constantBufferDesc,
		&subresourceData,
		&buffer
	));

	pBufferPtrMap->emplace(slotIndex, buffer);
}

void ShaderBoxNativeBridge::ShaderBoxMaterial::SetImageBuffer(const UINT slotIndex, const int shaderType, const std::wstring& path)
{
	std::unordered_map<UINT, Unsafe_Shared_Ptr<TextureData>>* pBufferPtrMap = nullptr;
	switch (shaderType)
	{
	case 0:
		pBufferPtrMap = &m_VertexTextureDataPtrMap;
		break;
	case 1:
		pBufferPtrMap = &m_HullTextureDataPtrMap;
		break;
	case 2:
		pBufferPtrMap = &m_DomainTextureDataPtrMap;
		break;
	case 3:
		pBufferPtrMap = &m_GeometryTextureDataPtrMap;
		break;
	case 4:
		pBufferPtrMap = &m_PixelTextureDataPtrMap;
		break;
	}

	const std::unordered_map<UINT, Unsafe_Shared_Ptr<TextureData>>::iterator it = pBufferPtrMap->find(slotIndex);
	if (it != pBufferPtrMap->cend())
	{
		// Update existing
		if (path.empty())
		{
			it->second = nullptr;
		}
		else
		{
			it->second = ContentManager::Load<TextureData>({ path });
		}
		return;
	}

	if (path.empty())
	{
		pBufferPtrMap->emplace(slotIndex, nullptr);
	}
	else
	{
		pBufferPtrMap->emplace(slotIndex, ContentManager::Load<TextureData>({ path }));
	}
}

void ShaderBoxNativeBridge::ShaderBoxMaterial::SetRasterizerState(const int cullMode, const int fillMode)
{
	if (m_pRasterizerState != nullptr)
	{
		m_pRasterizerState->Release();
	}
	if (cullMode == 0 && fillMode == 0)
	{
		m_pRasterizerState = nullptr;
		return;
	}

	D3D11_RASTERIZER_DESC desc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
	switch (cullMode)
	{
	case 0:
		desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
		break;
	case 1:
		desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
		break;
	case 2:
		desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
		break;
	}

	switch (fillMode)
	{
	case 0:
		desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		break;
	case 1:
		desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
		break;
	}

	CHECK_HR(SpringWind::GetInstance()->GetEngineContext().pDevice->CreateRasterizerState(&desc, &m_pRasterizerState));
}
