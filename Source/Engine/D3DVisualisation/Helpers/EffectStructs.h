#pragma once
#include "stdafx.h"

namespace SpringWindEngine
{
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

	struct ILDescription final
	{
		ILSemantic SemanticType;
		UINT SemanticIndex;
		UINT Offset;
	};

	struct InputLayoutDesc final
	{
		ID3D11InputLayout* pInputLayout = nullptr;
		std::vector<ILDescription> InputLayoutDescriptions;
		UINT InputLayoutSize;
	};

	struct Effect final
	{
		InputLayoutDesc* pInputLayoutDesc = nullptr;
		ID3D11VertexShader* pVertexShader = nullptr;
		ID3D11HullShader* pHullShader = nullptr;
		ID3D11DomainShader* pDomainShader = nullptr;
		ID3D11GeometryShader* pGeometryShader = nullptr;
		ID3D11PixelShader* pPixelShader = nullptr;

		D3D11_PRIMITIVE_TOPOLOGY Topology;
	private:
		Effect() {};

		friend class SpriteRenderer;
		friend class EffectFactory;
		friend class Material;
		
		int Counter = 0;
		size_t Hash = 0;
	};
}