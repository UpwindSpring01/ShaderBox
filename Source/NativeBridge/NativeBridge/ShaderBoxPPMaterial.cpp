#include "stdafx.h"
#include "ShaderBoxPPMaterial.h"

using namespace SpringWindEngine;

std::vector<ShaderBoxNativeBridge::ShaderBoxPPMaterial*> ShaderBoxNativeBridge::ShaderBoxPPMaterial::s_ShaderBoxMatsPtrVec;

ShaderBoxNativeBridge::ShaderBoxPPMaterial::ShaderBoxPPMaterial(const std::wstring& pixelShader) 
	: PPMaterial(pixelShader)
{
	s_ShaderBoxMatsPtrVec.push_back(this);
}

ShaderBoxNativeBridge::ShaderBoxPPMaterial::~ShaderBoxPPMaterial()
{
	s_ShaderBoxMatsPtrVec.erase(std::find(s_ShaderBoxMatsPtrVec.cbegin(), s_ShaderBoxMatsPtrVec.cend(), this));
	for (const std::pair<UINT, ID3D11Buffer*>& kvp : m_PixelBufferPtrMap)
	{
		kvp.second->Release();
	}
}

void ShaderBoxNativeBridge::ShaderBoxPPMaterial::BeginDraw()
{
	ID3D11DeviceContext* pDeviceContext = SpringWind::GetInstance()->GetEngineContext().pDeviceContext;

	for (const std::pair<UINT, ID3D11Buffer*>& kvp : m_PixelBufferPtrMap)
	{
		pDeviceContext->PSSetConstantBuffers(kvp.first, 1, &kvp.second);
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
}

void ShaderBoxNativeBridge::ShaderBoxPPMaterial::EndDraw()
{
}

void ShaderBoxNativeBridge::ShaderBoxPPMaterial::SetBuffer(const UINT slotIndex, const std::byte* data, const UINT length)
{
	std::unordered_map<UINT, ID3D11Buffer*>::const_iterator it = m_PixelBufferPtrMap.find(slotIndex);
	if (it != m_PixelBufferPtrMap.cend())
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

	m_PixelBufferPtrMap.emplace(slotIndex, buffer);
}

void ShaderBoxNativeBridge::ShaderBoxPPMaterial::SetImageBuffer(const UINT slotIndex, const std::wstring& path)
{
	const std::unordered_map<UINT, Unsafe_Shared_Ptr<TextureData>>::iterator it = m_PixelTextureDataPtrMap.find(slotIndex);
	if (it != m_PixelTextureDataPtrMap.cend())
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
		m_PixelTextureDataPtrMap.emplace(slotIndex, nullptr);
	}
	else
	{
		m_PixelTextureDataPtrMap.emplace(slotIndex, ContentManager::Load<TextureData>({ path }));
	}
	
}