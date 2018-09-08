#pragma once
#include "stdafx.h"

namespace SpringWindEngine
{
	struct BaseConstantBufferFrame
	{
		XMFLOAT4X4 view = {};
		XMFLOAT4X4 projection = {};
		XMFLOAT4X4 viewInverse = {};
		XMFLOAT3 lightDirection = XMFLOAT3(-0.577f, -0.577f, 0.577f);
	};

	struct BaseConstantBufferObject
	{
		XMFLOAT4X4 world = {};
		XMFLOAT4X4 worldViewProjection = {};
		XMFLOAT4X4 worldInverse = {};
	};

	struct WindowContext
	{
		GameTime* pGameTime = nullptr;
		UINT Width = 0;
		UINT Height = 0;
	};

	struct EngineContext
	{
		ID3D11Device* pDevice = nullptr;
		ID3D11DeviceContext* pDeviceContext = nullptr;
		IDXGIFactory2* pFactory = nullptr;

		BaseConstantBufferFrame* pBaseCBufferFrame = nullptr;
		BaseConstantBufferObject* pBaseCBufferObject = nullptr;
	};

	struct RenderTarget
	{
		RenderTarget() {}
		
		ID3D11RenderTargetView* pRenderTargetView = nullptr;
		ID3D11DepthStencilView* pDepthStencilView = nullptr;
		ID3D11ShaderResourceView* pShaderResourceView = nullptr;

		void Release()
		{
			SafeRelease(pRenderTargetView);
			SafeRelease(pDepthStencilView);
			SafeRelease(pShaderResourceView);
		}

		RenderTarget(const RenderTarget& obj) = delete;
		RenderTarget& operator=(const RenderTarget& obj) = delete;
	};

	struct VertexBufferData
	{
		VertexBufferData() :
			pDataStart(nullptr),
			pVertexBuffer(nullptr),
			BufferSize(0),
			VertexStride(0),
			VertexCount(0)
		{}

		void* pDataStart;
		ID3D11Buffer* pVertexBuffer;
		UINT BufferSize;
		UINT VertexStride;
		UINT VertexCount;

		void Destroy()
		{
			delete[] pDataStart;
			SafeRelease(pVertexBuffer);
		}
	};
}