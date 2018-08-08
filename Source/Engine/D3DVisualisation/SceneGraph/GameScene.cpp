#include "stdafx.h"
#include "GameScene.h"

#include "GameObject.h"
#include "../Graphics/SpriteRenderer.h"

constexpr UINT RoundUpMultiple16(const UINT numToRound)
{
	return (numToRound + 15) & ~(15); // Only works when multiple is a power of 2
}

GameScene::GameScene(const GameContext& gameContext)
{
	m_GameContext = gameContext;
	InitFrameAndObjectBuffers();
	InitSamplers();
	CreateVertexBuffer();
}

GameScene::~GameScene()
{
	for (GameObject* pObj : m_ChildArr)
	{
		delete pObj;
	}
	m_ChildArr.clear();

	for (UINT i = 0; i < SAMPLER_AMOUNT; ++i)
	{
		m_pSamplersArr[i]->Release();
	}

	m_pBaseCBufferFrameBuffer->Release();
	m_pBaseCBufferObjectBuffer->Release();

	delete m_pBaseCBufferFrameData;
	delete m_pBaseCBufferObjectData;
	
	m_pVertexBuffer->Release();
}

void GameScene::AddChild(GameObject* pObj)
{
	pObj->m_pScene = this;
	m_ChildArr.emplace_back(pObj);
}

void GameScene::RootUpdate()
{
	for(GameObject* pObj : m_ChildArr)
	{
		pObj->RootUpdate();
	}
}

void GameScene::Draw(const GameContext& gameContext)
{
	XMStoreFloat4x4(&m_pBaseCBufferFrameData->view, XMMatrixTranspose(gameContext.pCameraData->m_View));
	XMStoreFloat4x4(&m_pBaseCBufferFrameData->projection, XMMatrixTranspose(gameContext.pCameraData->m_Projection));
	XMStoreFloat4x4(&m_pBaseCBufferFrameData->viewInverse, XMMatrixTranspose(gameContext.pCameraData->m_ViewInv));

	D3D11_MAPPED_SUBRESOURCE resource;
	gameContext.pDeviceContext->Map(m_pBaseCBufferFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, m_pBaseCBufferFrameData, sizeof(BaseConstantBufferFrame));
	gameContext.pDeviceContext->Unmap(m_pBaseCBufferFrameBuffer, 0);
	
	for(GameObject* pObj : m_ChildArr)
	{
		pObj->RootDraw(gameContext);
	}

	if (m_pPPEffect != nullptr)
	{
		SpringWind::RenderTarget& rt = SpringWind::GetInstance()->GetRenderTarget();
		SpringWind::RenderTarget& ppRt = SpringWind::GetInstance()->GetPPRenderTarget();

		m_GameContext.pDeviceContext->OMSetRenderTargets(1, &ppRt.m_pRenderTargetView, ppRt.m_pDepthStencilView);

		static float ClearColor[4] = { 0.0f, 0.2f, 0.2f, 1.0f };
		m_GameContext.pDeviceContext->ClearRenderTargetView(ppRt.m_pRenderTargetView, ClearColor);
		m_GameContext.pDeviceContext->ClearDepthStencilView(ppRt.m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		UINT offset = 0;
		UINT stride = sizeof(VertexPosTex);
		m_GameContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
		m_GameContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		m_GameContext.pDeviceContext->IASetInputLayout(m_pPPEffect->InputLayoutPtr);

		m_GameContext.pDeviceContext->VSSetShader(m_pPPEffect->VertexShaderPtr, 0, 0);
		m_GameContext.pDeviceContext->PSSetShader(m_pPPEffect->PixelShaderPtr, 0, 0);
		m_GameContext.pDeviceContext->GSSetShader(nullptr, 0, 0);
		m_GameContext.pDeviceContext->HSSetShader(nullptr, 0, 0);
		m_GameContext.pDeviceContext->DSSetShader(nullptr, 0, 0);

		m_GameContext.pDeviceContext->PSSetShaderResources(0, 1, &rt.m_pShaderResourceView);

		m_GameContext.pDeviceContext->Draw(4, 0);

		ID3D11ShaderResourceView* pSRV = nullptr;
		m_GameContext.pDeviceContext->PSSetShaderResources(0, 1, &pSRV);
		m_GameContext.pDeviceContext->OMSetRenderTargets(1, &rt.m_pRenderTargetView, rt.m_pDepthStencilView);

		m_GameContext.pDeviceContext->ClearDepthStencilView(rt.m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		SpriteRenderer::GetInstance()->DrawImmediate(gameContext, ppRt.m_pShaderResourceView, XMFLOAT2(0, 0), XMFLOAT4(1, 1, 1, 1), XMFLOAT2(0, 0), XMFLOAT2(1, 1), 0);
	}
}

void GameScene::CreateVertexBuffer()
{
	//Create a vertex buffer for a full screen quad. Use the VertexPosTex struct (m_pVertexBuffer)
	D3D11_BUFFER_DESC vertexBuffDesc;

	vertexBuffDesc.ByteWidth = sizeof(VertexPosTex) * 4;
	vertexBuffDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	vertexBuffDesc.CPUAccessFlags = 0;
	vertexBuffDesc.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
	vertexBuffDesc.MiscFlags = 0;
	QuadPosTex quad(
		VertexPosTex(XMFLOAT3(-1, 1, 0), XMFLOAT2(0, 0)), // left top
		VertexPosTex(XMFLOAT3(1, 1, 0), XMFLOAT2(1, 0)), // right top
		VertexPosTex(XMFLOAT3(-1, -1, 0), XMFLOAT2(0, 1)), // left bottom
		VertexPosTex(XMFLOAT3(1, -1, 0), XMFLOAT2(1, 1))); // right bottom
	D3D11_SUBRESOURCE_DATA pData;
	pData.pSysMem = &quad;
	CHECK_HR(m_GameContext.pDevice->CreateBuffer(&vertexBuffDesc, &pData, &m_pVertexBuffer));
}


void GameScene::AddPPShader(Effect* pEffect)
{
	m_pPPEffect = pEffect;
}

void GameScene::UpdateObjectBuffer(const GameContext& gameContext)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	gameContext.pDeviceContext->Map(m_pBaseCBufferObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, m_pBaseCBufferObjectData, sizeof(BaseConstantBufferObject));
	gameContext.pDeviceContext->Unmap(m_pBaseCBufferObjectBuffer, 0);
}

void GameScene::InitFrameAndObjectBuffers()
{
	ID3D11Device* pDevice = m_GameContext.pDevice;
	

	// Initialize buffer for cbuffer variables, which stays the same for al draw calls during one frame
	D3D11_BUFFER_DESC constantBufferDesc = {};
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.ByteWidth = RoundUpMultiple16(sizeof(BaseConstantBufferFrame));
	CHECK_HR(pDevice->CreateBuffer(
		&constantBufferDesc,
		nullptr,             // leave the buffer uninitialized
		&m_pBaseCBufferFrameBuffer
	));
	m_pBaseCBufferFrameData = new BaseConstantBufferFrame();

	constantBufferDesc.ByteWidth = sizeof(BaseConstantBufferObject);
	CHECK_HR(pDevice->CreateBuffer(
		&constantBufferDesc,
		nullptr,             // leave the buffer uninitialized
		&m_pBaseCBufferObjectBuffer
	));
	m_pBaseCBufferObjectData = new BaseConstantBufferObject();

	BindBuffers();
}

void GameScene::BindBuffers()
{
	ID3D11DeviceContext* pDeviceContext = m_GameContext.pDeviceContext;
	pDeviceContext->VSSetConstantBuffers(0, 1, &m_pBaseCBufferFrameBuffer);
	pDeviceContext->VSSetConstantBuffers(1, 1, &m_pBaseCBufferObjectBuffer);

	pDeviceContext->HSSetConstantBuffers(0, 1, &m_pBaseCBufferFrameBuffer);
	pDeviceContext->HSSetConstantBuffers(1, 1, &m_pBaseCBufferObjectBuffer);

	pDeviceContext->DSSetConstantBuffers(0, 1, &m_pBaseCBufferFrameBuffer);
	pDeviceContext->DSSetConstantBuffers(1, 1, &m_pBaseCBufferObjectBuffer);

	pDeviceContext->GSSetConstantBuffers(0, 1, &m_pBaseCBufferFrameBuffer);
	pDeviceContext->GSSetConstantBuffers(1, 1, &m_pBaseCBufferObjectBuffer);

	pDeviceContext->PSSetConstantBuffers(0, 1, &m_pBaseCBufferFrameBuffer);
	pDeviceContext->PSSetConstantBuffers(1, 1, &m_pBaseCBufferObjectBuffer);
}

void GameScene::InitSamplers()
{
	ID3D11Device* pDevice = m_GameContext.pDevice;
	ID3D11DeviceContext* pDeviceContext = m_GameContext.pDeviceContext;

	D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());
	samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	CHECK_HR(pDevice->CreateSamplerState(&samplerDesc, &m_pSamplersArr[0]));

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_MIRROR;
	CHECK_HR(pDevice->CreateSamplerState(&samplerDesc, &m_pSamplersArr[1]));

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
	CHECK_HR(pDevice->CreateSamplerState(&samplerDesc, &m_pSamplersArr[2]));

	samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
	CHECK_HR(pDevice->CreateSamplerState(&samplerDesc, &m_pSamplersArr[3]));

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_MIRROR;
	CHECK_HR(pDevice->CreateSamplerState(&samplerDesc, &m_pSamplersArr[4]));

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
	CHECK_HR(pDevice->CreateSamplerState(&samplerDesc, &m_pSamplersArr[5]));

	pDeviceContext->VSSetSamplers(0, SAMPLER_AMOUNT, m_pSamplersArr);
	pDeviceContext->HSSetSamplers(0, SAMPLER_AMOUNT, m_pSamplersArr);
	pDeviceContext->DSSetSamplers(0, SAMPLER_AMOUNT, m_pSamplersArr);
	pDeviceContext->GSSetSamplers(0, SAMPLER_AMOUNT, m_pSamplersArr);
	pDeviceContext->PSSetSamplers(0, SAMPLER_AMOUNT, m_pSamplersArr);
}
