#include "stdafx.h"

#include "EffectHelper.h"

HRESULT EffectHelper::BuildInputLayout(ID3D11Device* pDevice, const byte * data, const UINT length, ID3D11InputLayout **pInputLayout, std::vector<ILDescription>& inputLayoutDescriptions, UINT& inputLayoutSize, UINT& inputLayoutID)
{
	ID3D11ShaderReflection* pVertexShaderReflection = nullptr;
	CHECK_HR(D3DReflect(data, length, IID_ID3D11ShaderReflection, (void**)&pVertexShaderReflection));

	// Get shader info
	D3D11_SHADER_DESC shaderDesc;
	pVertexShaderReflection->GetDesc(&shaderDesc);

	inputLayoutSize = 0;
	inputLayoutDescriptions.clear();
	// Read input layout description from shader info
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (UINT i = 0; i< shaderDesc.InputParameters; ++i)
	{
		ILDescription ilDescription = {};

		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		CHECK_HR(pVertexShaderReflection->GetInputParameterDesc(i, &paramDesc));

		ilDescription.Offset = static_cast<UINT>(floor(log(paramDesc.Mask) / log(2)) + 1) * 4;

		// fill out input element desc
		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		// determine DXGI format
		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (paramDesc.Mask <= 3)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (paramDesc.Mask <= 7)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (paramDesc.Mask <= 15)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		}

		if (strcmp(paramDesc.SemanticName, "POSITION") == 0)ilDescription.SemanticType = ILSemantic::POSITION;
		else if (strcmp(paramDesc.SemanticName, "NORMAL") == 0)ilDescription.SemanticType = ILSemantic::NORMAL;
		else if (strcmp(paramDesc.SemanticName, "TEXCOORD") == 0)ilDescription.SemanticType = ILSemantic::TEXCOORD;
		else {
			ilDescription.SemanticType = ILSemantic::NONE;
		}

		//Enable corresponding BitFlag
		//inputLayoutID += ilDescription.SemanticType * static_cast<UINT>(pow(10, i));
		inputLayoutID |= ilDescription.SemanticType;

		//wchar_t buffer[50];
		//wsprintf(buffer,L"This is a test with %d results",5);
		//Logger::Log(wstring((wchar_t*)&buffer),LogLevel::Error);
		//Semantic Index
		ilDescription.SemanticIndex = paramDesc.SemanticIndex;

		//Add Description to Vector
		inputLayoutDescriptions.push_back(ilDescription);

		//save element desc
		inputLayoutDesc.push_back(elementDesc);

		inputLayoutSize += ilDescription.Offset;
	}

	// Try to create Input Layout
	HRESULT hr = pDevice->CreateInputLayout(&inputLayoutDesc[0], (UINT)inputLayoutDesc.size(), data, length, pInputLayout);

	//Free allocation shader reflection memory
	pVertexShaderReflection->Release();

	return hr;
}