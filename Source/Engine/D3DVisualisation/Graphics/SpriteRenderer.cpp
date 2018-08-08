//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "SpriteRenderer.h"
#include "../Content/ContentManager.h"
#include "../Helpers/EffectHelper.h"
#include "../Content/EffectLoader.h"
#include "TextureData.h"
#include "../Scenegraph/GameScene.h"


SpriteRenderer::SpriteRenderer() :
	m_pImmediateVertexBuffer(nullptr)
{
	//Effect
	m_pEffect = ContentManager::Load<Effect>({ L".InternalSprite", L"./Resources/.Internal/CSO/Sprite_VS.cso", L"", L"", L"./Resources/.Internal/CSO/Sprite_GS.cso", L"./Resources/.Internal/CSO/Sprite_PS.cso" });

	D3D11_BUFFER_DESC constantBufferDesc = {};
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.ByteWidth = 16;
	CHECK_HR(SpringWind::GetInstance()->GetGameContext().pDevice->CreateBuffer(
		&constantBufferDesc,
		nullptr,
		&m_pTextureSizeBuffer
	));
}


SpriteRenderer::~SpriteRenderer()
{
	SafeRelease(m_pImmediateVertexBuffer);
	SafeRelease(m_pTextureSizeBuffer);
}

void SpriteRenderer::DrawImmediate(const GameContext& gameContext, ID3D11ShaderResourceView* pSrv, XMFLOAT2 position, XMFLOAT4 color, XMFLOAT2 pivot, XMFLOAT2 scale, float rotation)
{
	//Create Immediate VB
	if (!m_pImmediateVertexBuffer)
	{
		//Create a new buffer
		D3D11_BUFFER_DESC buffDesc;
		buffDesc.Usage = D3D11_USAGE_DYNAMIC;
		buffDesc.ByteWidth = sizeof(SpriteVertex);
		buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffDesc.MiscFlags = 0;

		CHECK_HR(gameContext.pDevice->CreateBuffer(&buffDesc, nullptr, &m_pImmediateVertexBuffer));
	}

	//Map Vertex
	SpriteVertex vertex;
	vertex.TextureId = 0;
	vertex.TransformData = XMFLOAT4(position.x, position.y, 0, rotation);
	vertex.TransformData2 = XMFLOAT4(pivot.x, pivot.y, scale.x, scale.y);
	vertex.Color = color;

	if (!m_ImmediateVertex.Equals(vertex))
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		gameContext.pDeviceContext->Map(m_pImmediateVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, &vertex, sizeof(SpriteVertex));
		gameContext.pDeviceContext->Unmap(m_pImmediateVertexBuffer, 0);
		m_ImmediateVertex = vertex;
	}

	//Set Render Pipeline
	gameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	UINT stride = sizeof(SpriteVertex);
	UINT offset = 0;
	gameContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pImmediateVertexBuffer, &stride, &offset);
	gameContext.pDeviceContext->IASetInputLayout(m_pEffect->InputLayoutPtr);

	gameContext.pDeviceContext->VSSetShader(m_pEffect->VertexShaderPtr, 0, 0);
	gameContext.pDeviceContext->PSSetShader(m_pEffect->PixelShaderPtr, 0, 0);
	gameContext.pDeviceContext->HSSetShader(nullptr, 0, 0);
	gameContext.pDeviceContext->DSSetShader(nullptr, 0, 0);
	gameContext.pDeviceContext->GSSetShader(m_pEffect->GeometryShaderPtr, 0, 0);

	gameContext.pDeviceContext->PSSetShaderResources(0, 1, &pSrv);

	float scaleX = (SpringWind::GetInstance()->GetWidth() > 0) ? 2.0f / SpringWind::GetInstance()->GetWidth() : 0;
	float scaleY = (SpringWind::GetInstance()->GetHeight() > 0) ? 2.0f / SpringWind::GetInstance()->GetHeight() : 0;

	m_Transform._11 = scaleX; m_Transform._12 = 0; m_Transform._13 = 0; m_Transform._14 = 0;
	m_Transform._21 = 0; m_Transform._22 = -scaleY; m_Transform._23 = 0; m_Transform._24 = 0;
	m_Transform._31 = 0; m_Transform._32 = 0; m_Transform._33 = 1; m_Transform._34 = 0;
	m_Transform._41 = -1; m_Transform._42 = 1; m_Transform._43 = 0; m_Transform._44 = 1;
	XMStoreFloat4x4(&m_Transform, XMMatrixTranspose(XMLoadFloat4x4(&m_Transform)));

	SpringWind::GetInstance()->GetScene()->GetObjectDataCBuffer()->worldViewProjection = m_Transform;
	SpringWind::GetInstance()->GetScene()->UpdateObjectBuffer(gameContext);

	gameContext.pDeviceContext->GSSetConstantBuffers(2, 1, &m_pTextureSizeBuffer);

	ID3D11Resource* pResource;
	pSrv->GetResource(&pResource);
	D3D11_TEXTURE2D_DESC texDesc;
	ID3D11Texture2D* texResource = reinterpret_cast<ID3D11Texture2D*>(pResource);
	texResource->GetDesc(&texDesc);
	texResource->Release();

	XMFLOAT2 texSize = XMFLOAT2(static_cast<float>(texDesc.Width), static_cast<float>(texDesc.Height));
	D3D11_MAPPED_SUBRESOURCE resource;
	gameContext.pDeviceContext->Map(m_pTextureSizeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, &texSize, sizeof(XMFLOAT2));
	gameContext.pDeviceContext->Unmap(m_pTextureSizeBuffer, 0);

	gameContext.pDeviceContext->Draw(1, 0);
}

