#pragma once
#include "stdafx.h"

struct CameraData
{
	XMMATRIX m_Projection;
	XMMATRIX m_View;
	XMMATRIX m_ViewInv;
};

struct GameContext
{
	GameTime* pGameTime = nullptr;
	ID3D11Device* pDevice = nullptr;
	ID3D11DeviceContext* pDeviceContext = nullptr;
	CameraData* pCameraData = nullptr;
};

enum ILSemantic : UINT
{
	NONE = 0,
	POSITION = (1 << 0),
	NORMAL = (1 << 1),
	COLOR = (1 << 2),
	TEXCOORD = (1 << 3),
	BINORMAL = (1 << 4),
	TANGENT = (1 << 5),
};

struct ILDescription
{
	ILSemantic SemanticType;
	DXGI_FORMAT Format;
	UINT SemanticIndex;
	UINT Offset;
};

struct Effect {
	ID3D11InputLayout* InputLayoutPtr = nullptr;
	ID3D11VertexShader* VertexShaderPtr = nullptr;
	ID3D11HullShader* HullShaderPtr = nullptr;
	ID3D11DomainShader* DomainShaderPtr = nullptr;
	ID3D11GeometryShader* GeometryShaderPtr = nullptr;
	ID3D11PixelShader* PixelShaderPtr = nullptr;

	std::vector<ILDescription> m_pInputLayoutDescriptions;
	UINT m_pInputLayoutSize;
	UINT m_InputLayoutID;

	D3D11_PRIMITIVE_TOPOLOGY m_Topology;

	~Effect()
	{
		Release();
	}

private:
	friend class EffectLoader;
	void Release()
	{
		SafeRelease(InputLayoutPtr);
		SafeRelease(VertexShaderPtr);
		SafeRelease(HullShaderPtr);
		SafeRelease(DomainShaderPtr);
		SafeRelease(GeometryShaderPtr);
		SafeRelease(PixelShaderPtr);
	}
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