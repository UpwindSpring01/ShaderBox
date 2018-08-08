#include "stdafx.h"

#include "Material.h"
#include "../Content/EffectLoader.h"

Material::Material()
{
}

Material::~Material()
{
}

void Material::SetEffect(Effect* pEffect)
{
	m_pEffect = pEffect;
}

void Material::SetShadersAndBuffers(const GameContext& gameContext, const TransformComponent* /*pTransComp*/)
{
	ID3D11DeviceContext* pDeviceContext = gameContext.pDeviceContext;
	pDeviceContext->IASetInputLayout(m_pEffect->InputLayoutPtr);
	pDeviceContext->VSSetShader(m_pEffect->VertexShaderPtr, 0, 0);
	pDeviceContext->PSSetShader(m_pEffect->PixelShaderPtr, 0, 0);
	pDeviceContext->GSSetShader(m_pEffect->GeometryShaderPtr, 0, 0);
	pDeviceContext->HSSetShader(m_pEffect->HullShaderPtr, 0, 0);
	pDeviceContext->DSSetShader(m_pEffect->DomainShaderPtr, 0, 0);
}

void Material::UpdateBuffers(const UINT cbufferIndex, const int shaderType, const byte* data, const UINT length)
{
	ID3D11Device* pDevice = SpringWind::GetInstance()->GetGameContext().pDevice;
	ID3D11DeviceContext* pDeviceContext = SpringWind::GetInstance()->GetGameContext().pDeviceContext;

	D3D11_BUFFER_DESC constantBufferDesc = {};
	constantBufferDesc.StructureByteStride = 0;
	constantBufferDesc.MiscFlags = 0;

	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	constantBufferDesc.CPUAccessFlags = 0;
	constantBufferDesc.ByteWidth = length;

	ID3D11Buffer* buffer;
	D3D11_SUBRESOURCE_DATA subresourceData;
	subresourceData.pSysMem = data;

	CHECK_HR(pDevice->CreateBuffer(
		&constantBufferDesc,
		&subresourceData,
		&buffer
	));
	
	switch (shaderType)
	{
	case 0:
		pDeviceContext->VSSetConstantBuffers(cbufferIndex, 1, &buffer);
		break;
	case 1:
		pDeviceContext->HSSetConstantBuffers(cbufferIndex, 1, &buffer);
		break;
	case 2:
		pDeviceContext->DSSetConstantBuffers(cbufferIndex, 1, &buffer);
		break;
	case 3:
		pDeviceContext->GSSetConstantBuffers(cbufferIndex, 1, &buffer);
		break;
	case 4:
		pDeviceContext->PSSetConstantBuffers(cbufferIndex, 1, &buffer);
		break;
	}

	buffer->Release();
}

void Material::UpdateImageBuffers(const UINT cbufferIndex, const int shaderType, const std::wstring path)
{
	ID3D11ShaderResourceView* pSRV = nullptr;
	if (!path.empty())
	{
		TextureData* pTexture = ContentManager::Load<TextureData>({ path });
		pSRV = pTexture->GetShaderResourceView();
	}

	ID3D11DeviceContext* pDeviceContext = SpringWind::GetInstance()->GetGameContext().pDeviceContext;

	switch (shaderType)
	{
	case 0:
		pDeviceContext->VSSetShaderResources(cbufferIndex, 1, &pSRV);
		break;
	case 1:
		pDeviceContext->HSSetShaderResources(cbufferIndex, 1, &pSRV);
		break;
	case 2:
		pDeviceContext->DSSetShaderResources(cbufferIndex, 1, &pSRV);
		break;
	case 3:
		pDeviceContext->GSSetShaderResources(cbufferIndex, 1, &pSRV);
		break;
	case 4:
		pDeviceContext->PSSetShaderResources(cbufferIndex, 1, &pSRV);
		break;
	}
}

