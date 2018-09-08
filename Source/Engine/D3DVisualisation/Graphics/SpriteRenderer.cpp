//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

#include "SpriteRenderer.h"
#include "Content/ContentManager.h"
#include "TextureData.h"
#include "Scenegraph/GameScene.h"
#include "Content/Shaders/EffectFactory.h"


SpringWindEngine::SpriteRenderer::SpriteRenderer() :
	m_pImmediateVertexBuffer(nullptr)
{
	m_pEffect = EffectFactory::GetInstance()->LoadEffect(L"./Resources/.Internal/CSO/Sprite_VS.cso", L"./Resources/.Internal/CSO/Sprite_PS.cso", L"./Resources/.Internal/CSO/Sprite_GS.cso");
	++m_pEffect->Counter;

	const EngineContext& engineContext = SpringWind::GetInstance()->GetEngineContext();
	D3D11_BUFFER_DESC constantBufferDesc = {};
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.ByteWidth = 16;
	CHECK_HR(engineContext.pDevice->CreateBuffer(&constantBufferDesc, nullptr, &m_pTextureSizeBuffer));

	//Create a new buffer
	D3D11_BUFFER_DESC buffDesc;
	buffDesc.Usage = D3D11_USAGE_DYNAMIC;
	buffDesc.ByteWidth = sizeof(SpriteVertex);
	buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffDesc.MiscFlags = 0;

	CHECK_HR(engineContext.pDevice->CreateBuffer(&buffDesc, nullptr, &m_pImmediateVertexBuffer));
}


SpringWindEngine::SpriteRenderer::~SpriteRenderer()
{
	--m_pEffect->Counter;

	SafeRelease(m_pImmediateVertexBuffer);
	SafeRelease(m_pTextureSizeBuffer);
}

void SpringWindEngine::SpriteRenderer::DrawImmediate(const EngineContext& engineContext, const WindowContext& windowContext,
	ID3D11ShaderResourceView* pSrv, XMFLOAT2 position, XMFLOAT4 color, XMFLOAT2 pivot, XMFLOAT2 scale, float rotation)
{
	//Map Vertex
	SpriteVertex vertex;
	vertex.TextureId = 0;
	vertex.TransformData = XMFLOAT4(position.x, position.y, 0, rotation);
	vertex.TransformData2 = XMFLOAT4(pivot.x, pivot.y, scale.x, scale.y);
	vertex.Color = color;

	if (!m_LastVertex.Equals(vertex))
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		engineContext.pDeviceContext->Map(m_pImmediateVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, &vertex, sizeof(SpriteVertex));
		engineContext.pDeviceContext->Unmap(m_pImmediateVertexBuffer, 0);
		m_LastVertex = vertex;
	}

	//Set Render Pipeline
	engineContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	UINT stride = sizeof(SpriteVertex);
	UINT offset = 0;
	engineContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pImmediateVertexBuffer, &stride, &offset);
	engineContext.pDeviceContext->IASetInputLayout(m_pEffect->pInputLayoutDesc->pInputLayout);

	engineContext.pDeviceContext->VSSetShader(m_pEffect->pVertexShader, 0, 0);
	engineContext.pDeviceContext->PSSetShader(m_pEffect->pPixelShader, 0, 0);
	engineContext.pDeviceContext->HSSetShader(nullptr, 0, 0);
	engineContext.pDeviceContext->DSSetShader(nullptr, 0, 0);
	engineContext.pDeviceContext->GSSetShader(m_pEffect->pGeometryShader, 0, 0);

	engineContext.pDeviceContext->PSSetShaderResources(0, 1, &pSrv);

	float scaleX = (windowContext.Width > 0) ? 2.0f / windowContext.Width : 0;
	float scaleY = (windowContext.Height > 0) ? 2.0f / windowContext.Height : 0;

	XMFLOAT4X4 transform(
		scaleX, 0, 0, -1,
		0, -scaleY, 0, 1,
		0, 0, 1, 0,
		0, 0, 0, 1);

	engineContext.pBaseCBufferObject->worldViewProjection = transform;
	SpringWind::GetInstance()->UpdateObjectBuffer();

	ID3D11Resource* pResource;
	pSrv->GetResource(&pResource);
	D3D11_TEXTURE2D_DESC texDesc;
	ID3D11Texture2D* texResource = reinterpret_cast<ID3D11Texture2D*>(pResource);
	texResource->GetDesc(&texDesc);
	texResource->Release();

	XMFLOAT2 texSize = XMFLOAT2(static_cast<float>(texDesc.Width), static_cast<float>(texDesc.Height));
	if (texSize != m_LastSize)
	{
		D3D11_MAPPED_SUBRESOURCE resource;
		engineContext.pDeviceContext->Map(m_pTextureSizeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		memcpy(resource.pData, &texSize, sizeof(XMFLOAT2));
		engineContext.pDeviceContext->Unmap(m_pTextureSizeBuffer, 0);
		
		m_LastSize = texSize;
	}
	engineContext.pDeviceContext->GSSetConstantBuffers(2, 1, &m_pTextureSizeBuffer);

	engineContext.pDeviceContext->Draw(1, 0);

	ID3D11ShaderResourceView* pNullSRV[] = { nullptr };
	engineContext.pDeviceContext->PSSetShaderResources(0, 1, pNullSRV);
}

