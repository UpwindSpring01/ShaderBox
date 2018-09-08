#include "stdafx.h"

#include "ShaderStorage.h"

SpringWindEngine::ShaderStorage::~ShaderStorage()
{
	for (const std::pair<size_t, std::pair<InputLayoutDesc*, UINT>>& kvp : m_InputLayoutPtrMap)
	{
		assert(kvp.second.second == 0);
		kvp.second.first->pInputLayout->Release();
		delete kvp.second.first;
	}
	for (const std::pair<size_t, std::pair<ID3D11VertexShader*, UINT>>& kvp : m_VertexShaderPtrMap)
	{
		assert(kvp.second.second == 0);
		kvp.second.first->Release();
	}
	for (const std::pair<size_t, std::pair<ID3D11HullShader*, UINT>>& kvp : m_HullShaderPtrMap)
	{
		assert(kvp.second.second == 0);
		kvp.second.first->Release();
	}
	for (const std::pair<size_t, std::pair<ID3D11DomainShader*, UINT>>& kvp : m_DomainShaderPtrMap)
	{
		assert(kvp.second.second == 0);
		kvp.second.first->Release();
	}
	for (const std::pair<size_t, std::pair<ID3D11GeometryShader*, UINT>>& kvp : m_GeometryShaderPtrMap)
	{
		assert(kvp.second.second == 0);
		kvp.second.first->Release();
	}
	for (const std::pair<size_t, std::pair<ID3D11PixelShader*, UINT>>& kvp : m_PixelShaderPtrMap)
	{
		assert(kvp.second.second == 0);
		kvp.second.first->Release();
	}
}

ID3D11VertexShader* SpringWindEngine::ShaderStorage::GetVertexShader(const std::wstring& assetFile, InputLayoutDesc** inputLayoutDesc)
{
	size_t seed = 0;
	hash_combine(seed, assetFile);
	const std::unordered_map<size_t, std::pair<ID3D11VertexShader*, UINT>>::iterator it = m_VertexShaderPtrMap.find(seed);
	if (it != m_VertexShaderPtrMap.cend())
	{
		if (inputLayoutDesc != nullptr)
		{
			UINT length;
			byte* data = LoadShaderFile(assetFile, length);
			*inputLayoutDesc = BuildInputLayout(SpringWind::GetInstance()->GetEngineContext().pDevice, data, length);
			delete[] data;
		}
		++(it->second.second);
		return it->second.first;
	}

	UINT length;
	byte* data = LoadShaderFile(assetFile, length);
	ID3D11VertexShader* pVertexS;
	ID3D11Device* pDevice = SpringWind::GetInstance()->GetEngineContext().pDevice;
	CHECK_HR(pDevice->CreateVertexShader(data, length, nullptr, &pVertexS));
	if (inputLayoutDesc != nullptr)
	{
		*inputLayoutDesc = BuildInputLayout(pDevice, data, length);
	}
	delete[] data;

	m_VertexShaderPtrMap.emplace(seed, std::pair(pVertexS, 1u));

	return pVertexS;
}

ID3D11HullShader* SpringWindEngine::ShaderStorage::GetHullShader(const std::wstring& assetFile)
{
	size_t seed = 0;
	hash_combine(seed, assetFile);
	const std::unordered_map<size_t, std::pair<ID3D11HullShader*, UINT>>::iterator it = m_HullShaderPtrMap.find(seed);
	if (it != m_HullShaderPtrMap.cend())
	{
		++(it->second.second);
		return it->second.first;
	}
	UINT length;
	byte* data = LoadShaderFile(assetFile, length);

	ID3D11HullShader* pHullS;
	ID3D11Device* pDevice = SpringWind::GetInstance()->GetEngineContext().pDevice;

	CHECK_HR(pDevice->CreateHullShader(data, length, nullptr, &pHullS));
	delete[] data;

	m_HullShaderPtrMap.emplace(seed, std::pair(pHullS, 1u));

	return pHullS;
}

ID3D11DomainShader* SpringWindEngine::ShaderStorage::GetDomainShader(const std::wstring& assetFile)
{
	size_t seed = 0;
	hash_combine(seed, assetFile);
	const std::unordered_map<size_t, std::pair<ID3D11DomainShader*, UINT>>::iterator it = m_DomainShaderPtrMap.find(seed);
	if (it != m_DomainShaderPtrMap.cend())
	{
		++(it->second.second);
		return it->second.first;
	}
	UINT length;
	byte* data = LoadShaderFile(assetFile, length);

	ID3D11DomainShader* pDomainS;
	ID3D11Device* pDevice = SpringWind::GetInstance()->GetEngineContext().pDevice;

	CHECK_HR(pDevice->CreateDomainShader(data, length, nullptr, &pDomainS));
	delete[] data;

	m_DomainShaderPtrMap.emplace(seed, std::pair(pDomainS, 1u));

	return pDomainS;
}

ID3D11GeometryShader* SpringWindEngine::ShaderStorage::GetGeometryShader(const std::wstring& assetFile)
{
	size_t seed = 0;
	hash_combine(seed, assetFile);
	const std::unordered_map<size_t, std::pair<ID3D11GeometryShader*, UINT>>::iterator it = m_GeometryShaderPtrMap.find(seed);
	if (it != m_GeometryShaderPtrMap.cend())
	{
		++(it->second.second);
		return it->second.first;
	}
	UINT length;
	byte* data = LoadShaderFile(assetFile, length);

	ID3D11GeometryShader* pGeometryS;
	ID3D11Device* pDevice = SpringWind::GetInstance()->GetEngineContext().pDevice;

	CHECK_HR(pDevice->CreateGeometryShader(data, length, nullptr, &pGeometryS));
	delete[] data;

	m_GeometryShaderPtrMap.emplace(seed, std::pair(pGeometryS, 1u));

	return pGeometryS;
}

ID3D11PixelShader* SpringWindEngine::ShaderStorage::GetPixelShader(const std::wstring& assetFile)
{
	size_t seed = 0;
	hash_combine(seed, assetFile);
	const std::unordered_map<size_t, std::pair<ID3D11PixelShader*, UINT>>::iterator it = m_PixelShaderPtrMap.find(seed);
	if (it != m_PixelShaderPtrMap.cend())
	{
		++(it->second.second);
		return it->second.first;
	}
	UINT length;
	byte* data = LoadShaderFile(assetFile, length);

	ID3D11PixelShader* pPixelS;
	ID3D11Device* pDevice = SpringWind::GetInstance()->GetEngineContext().pDevice;

	CHECK_HR(pDevice->CreatePixelShader(data, length, nullptr, &pPixelS));
	delete[] data;

	m_PixelShaderPtrMap.emplace(seed, std::pair(pPixelS, 1u));

	return pPixelS;
}

ID3D11VertexShader* SpringWindEngine::ShaderStorage::RefreshVertexShaderForced(const ID3D11VertexShader* pOldShader, const InputLayoutDesc* pOldInputLayoutDesc, const std::wstring& assetFile, InputLayoutDesc** inputLayoutDesc)
{
	if (pOldInputLayoutDesc != nullptr)
	{
		--std::find_if(m_InputLayoutPtrMap.begin(), m_InputLayoutPtrMap.end(),
			[pOldInputLayoutDesc](const std::pair<size_t, std::pair<InputLayoutDesc*, UINT>>& pair) { return pair.second.first == pOldInputLayoutDesc; }
		)->second.second;
	}

	std::unordered_map<size_t, std::pair<ID3D11VertexShader*, UINT>>::iterator it;
	if (pOldShader != nullptr)
	{
		it = std::find_if(m_VertexShaderPtrMap.begin(), m_VertexShaderPtrMap.end(),
			[pOldShader](const std::pair<size_t, std::pair<ID3D11VertexShader*, UINT>>& pair) { return pair.second.first == pOldShader; });
		it->second.first->Release();
	}
	size_t newHash = 0;
	hash_combine(newHash, assetFile);

	UINT length;
	byte* data = LoadShaderFile(assetFile, length);

	ID3D11Device* pDevice = SpringWind::GetInstance()->GetEngineContext().pDevice;
	ID3D11VertexShader* pVertexS;
	CHECK_HR(pDevice->CreateVertexShader(data, length, nullptr, &pVertexS));
	if (inputLayoutDesc != nullptr)
	{
		*inputLayoutDesc = BuildInputLayout(pDevice, data, length);
	}
	delete[] data;

	if (pOldShader != nullptr)
	{
		it->second.first = pVertexS;
	if (newHash != it->first)
	{
		auto pair = m_VertexShaderPtrMap.extract(it);
		pair.key() = newHash;
		m_VertexShaderPtrMap.insert(std::move(pair));
	}
	}
	else
	{
		m_VertexShaderPtrMap.emplace(newHash, std::pair(pVertexS, 1u));
	}

	return pVertexS;
}

ID3D11HullShader* SpringWindEngine::ShaderStorage::RefreshHullShaderForced(const ID3D11HullShader* pOldShader, const std::wstring& assetFile)
{
	std::unordered_map<size_t, std::pair<ID3D11HullShader*, UINT>>::iterator it;
	if (pOldShader != nullptr)
	{
		it = std::find_if(m_HullShaderPtrMap.begin(), m_HullShaderPtrMap.end(),
			[pOldShader](const std::pair<size_t, std::pair<ID3D11HullShader*, UINT>>& pair) { return pair.second.first == pOldShader; });
		it->second.first->Release();
	}
	size_t newHash = 0;
	hash_combine(newHash, assetFile);

	UINT length;
	byte* data = LoadShaderFile(assetFile, length);

	ID3D11Device* pDevice = SpringWind::GetInstance()->GetEngineContext().pDevice;
	ID3D11HullShader* pHullS;
	CHECK_HR(pDevice->CreateHullShader(data, length, nullptr, &pHullS));
	delete[] data;

	if (pOldShader != nullptr)
	{
		it->second.first = pHullS;
		if (newHash != it->first)
		{
			auto pair = m_HullShaderPtrMap.extract(it);
			pair.key() = newHash;
			m_HullShaderPtrMap.insert(std::move(pair));
		}
	}
	else
	{
		m_HullShaderPtrMap.emplace(newHash, std::pair(pHullS, 1u));
	}
	return pHullS;
}

ID3D11DomainShader* SpringWindEngine::ShaderStorage::RefreshDomainShaderForced(const ID3D11DomainShader* pOldShader, const std::wstring& assetFile)
{
	std::unordered_map<size_t, std::pair<ID3D11DomainShader*, UINT>>::iterator it;
	if (pOldShader != nullptr)
	{
		it = std::find_if(m_DomainShaderPtrMap.begin(), m_DomainShaderPtrMap.end(),
			[pOldShader](const std::pair<size_t, std::pair<ID3D11DomainShader*, UINT>>& pair) { return pair.second.first == pOldShader; });
		it->second.first->Release();
	}
	size_t newHash = 0;
	hash_combine(newHash, assetFile);

	UINT length;
	byte* data = LoadShaderFile(assetFile, length);

	ID3D11Device* pDevice = SpringWind::GetInstance()->GetEngineContext().pDevice;
	ID3D11DomainShader* pDomainS;
	CHECK_HR(pDevice->CreateDomainShader(data, length, nullptr, &pDomainS));
	delete[] data;

	if (pOldShader != nullptr)
	{
		it->second.first = pDomainS;
		if (newHash != it->first)
		{
			auto pair = m_DomainShaderPtrMap.extract(it);
			pair.key() = newHash;
			m_DomainShaderPtrMap.insert(std::move(pair));
		}
	}
	else
	{
		m_DomainShaderPtrMap.emplace(newHash, std::pair(pDomainS, 1u));
	}
	return pDomainS;
}

ID3D11GeometryShader* SpringWindEngine::ShaderStorage::RefreshGeometryShaderForced(const ID3D11GeometryShader* pOldShader, const std::wstring& assetFile)
{
	std::unordered_map<size_t, std::pair<ID3D11GeometryShader*, UINT>>::iterator it;
	if (pOldShader != nullptr)
	{
		it = std::find_if(m_GeometryShaderPtrMap.begin(), m_GeometryShaderPtrMap.end(),
			[pOldShader](const std::pair<size_t, std::pair<ID3D11GeometryShader*, UINT>>& pair) { return pair.second.first == pOldShader; });
		it->second.first->Release();
	}

	size_t newHash = 0;
	hash_combine(newHash, assetFile);

	UINT length;
	byte* data = LoadShaderFile(assetFile, length);

	ID3D11Device* pDevice = SpringWind::GetInstance()->GetEngineContext().pDevice;
	ID3D11GeometryShader* pGeometryS;
	CHECK_HR(pDevice->CreateGeometryShader(data, length, nullptr, &pGeometryS));
	delete[] data;

	if (pOldShader != nullptr)
	{
		it->second.first = pGeometryS;
		if (newHash != it->first)
		{
			auto pair = m_GeometryShaderPtrMap.extract(it);
			pair.key() = newHash;
			m_GeometryShaderPtrMap.insert(std::move(pair));
		}
	}
	else
	{
		m_GeometryShaderPtrMap.emplace(newHash, std::pair(pGeometryS, 1u));
	}

	return pGeometryS;
}

ID3D11PixelShader* SpringWindEngine::ShaderStorage::RefreshPixelShaderForced(const ID3D11PixelShader* pOldShader, const std::wstring& assetFile)
{
	std::unordered_map<size_t, std::pair<ID3D11PixelShader*, UINT>>::iterator it;
	if (pOldShader != nullptr)
	{
		it = std::find_if(m_PixelShaderPtrMap.begin(), m_PixelShaderPtrMap.end(),
			[pOldShader](const std::pair<size_t, std::pair<ID3D11PixelShader*, UINT>>& pair) { return pair.second.first == pOldShader; });
		it->second.first->Release();
	}

	size_t newHash = 0;
	hash_combine(newHash, assetFile);

	UINT length;
	byte* data = LoadShaderFile(assetFile, length);

	ID3D11Device* pDevice = SpringWind::GetInstance()->GetEngineContext().pDevice;
	ID3D11PixelShader* pPixelS;
	CHECK_HR(pDevice->CreatePixelShader(data, length, nullptr, &pPixelS));
	delete[] data;

	if (pOldShader != nullptr)
	{
		it->second.first = pPixelS;
		if (newHash != it->first)
		{
			auto pair = m_PixelShaderPtrMap.extract(it);
			pair.key() = newHash;
			m_PixelShaderPtrMap.insert(std::move(pair));
		}
	}
	else
	{
		m_PixelShaderPtrMap.emplace(newHash, std::pair(pPixelS, 1u));
	}

	return pPixelS;
}

byte* SpringWindEngine::ShaderStorage::LoadShaderFile(const std::wstring file, UINT& length)
{
	byte *pFileData = nullptr;

	// open the file
	std::ifstream fileStream(file, std::ios::in | std::ios::binary | std::ios::ate);

	// if open was successful
#ifdef _DEBUG
	if (!fileStream.is_open())
	{
		Logger::LogError(L"EffectLoader::LoadShaderFile -> File not found: " + file);
	}
#endif // _DEBUG


	// find the length of the file
	length = (int)fileStream.tellg();

	// collect the file data
	pFileData = new byte[length];
	fileStream.seekg(0, std::ios::beg);
	fileStream.read(reinterpret_cast<char*>(pFileData), length);
	fileStream.close();

	return pFileData;
}

SpringWindEngine::InputLayoutDesc* SpringWindEngine::ShaderStorage::BuildInputLayout(ID3D11Device* pDevice, const byte* data, const UINT length)
{
	ID3D11ShaderReflection* pVertexShaderReflection = nullptr;
	CHECK_HR(D3DReflect(data, length, IID_ID3D11ShaderReflection, (void**)&pVertexShaderReflection));

	// Get shader info
	D3D11_SHADER_DESC shaderDesc;
	pVertexShaderReflection->GetDesc(&shaderDesc);

	UINT inputLayoutSize = 0;
	size_t inputLayoutID = 0;
	std::vector<ILDescription> inputLayoutDescriptions;

	// Read input layout description from shader info
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (UINT i = 0; i < shaderDesc.InputParameters; ++i)
	{
		ILDescription ilDescription = {};

		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		CHECK_HR(pVertexShaderReflection->GetInputParameterDesc(i, &paramDesc));

		ilDescription.Offset = static_cast<UINT>(floor(log(paramDesc.Mask) / log(2)) + 1) * 4;
		ilDescription.SemanticIndex = paramDesc.SemanticIndex;

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
		else if (strcmp(paramDesc.SemanticName, "COLOR") == 0)ilDescription.SemanticType = ILSemantic::COLOR;
		else
		{
			ilDescription.SemanticType = ILSemantic::NONE;
		}

		hash_combine(inputLayoutID, ilDescription.SemanticType);
		hash_combine(inputLayoutID, ilDescription.SemanticIndex);

		//Add Description to Vector
		inputLayoutDescriptions.push_back(ilDescription);

		//save element desc
		inputLayoutDesc.push_back(elementDesc);

		inputLayoutSize += ilDescription.Offset;
	}

	std::unordered_map<size_t, std::pair<InputLayoutDesc*, UINT>>::iterator it = m_InputLayoutPtrMap.find(inputLayoutID);
	if (it != m_InputLayoutPtrMap.cend())
	{
		pVertexShaderReflection->Release();
		++(it->second.second);
		return it->second.first;
	}

	// Create Input Layout
	InputLayoutDesc* pIL = new InputLayoutDesc();
	pIL->InputLayoutDescriptions = inputLayoutDescriptions;
	pIL->InputLayoutSize = inputLayoutSize;
	CHECK_HR(pDevice->CreateInputLayout(&inputLayoutDesc[0], (UINT)inputLayoutDesc.size(), data, length, &pIL->pInputLayout));
	pVertexShaderReflection->Release();

	m_InputLayoutPtrMap.emplace(inputLayoutID, std::pair(pIL, 1u));

	return pIL;
}

void SpringWindEngine::ShaderStorage::UnloadVertexShader(const ID3D11VertexShader* pVertexShader, const InputLayoutDesc* pInputLayoutDesc)
{
	--std::find_if(m_VertexShaderPtrMap.begin(), m_VertexShaderPtrMap.end(),
		[pVertexShader](const std::pair<size_t, std::pair<ID3D11VertexShader*, UINT>>& pair) { return pair.second.first == pVertexShader; }
	)->second.second;

	if (pInputLayoutDesc != nullptr)
	{
		--std::find_if(m_InputLayoutPtrMap.begin(), m_InputLayoutPtrMap.end(),
			[pInputLayoutDesc](const std::pair<size_t, std::pair<InputLayoutDesc*, UINT>>& pair) { return pair.second.first == pInputLayoutDesc; }
		)->second.second;
	}
}

void SpringWindEngine::ShaderStorage::UnloadPixelShader(const ID3D11PixelShader* pPixelShader)
{
	--std::find_if(m_PixelShaderPtrMap.begin(), m_PixelShaderPtrMap.end(),
		[pPixelShader](const std::pair<size_t, std::pair<ID3D11PixelShader*, UINT>>& pair) { return pair.second.first == pPixelShader; }
	)->second.second;
}

void SpringWindEngine::ShaderStorage::UnloadEffectShaders(const Effect* pEffect)
{
	UnloadVertexShader(pEffect->pVertexShader, pEffect->pInputLayoutDesc);

	const ID3D11HullShader* pHullShader = pEffect->pHullShader;
	if (pHullShader != nullptr)
	{
		--std::find_if(m_HullShaderPtrMap.begin(), m_HullShaderPtrMap.end(),
			[pHullShader](const std::pair<size_t, std::pair<ID3D11HullShader*, UINT>>& pair) { return pair.second.first == pHullShader; }
		)->second.second;
	}

	const ID3D11DomainShader* pDomainShader = pEffect->pDomainShader;
	if (pDomainShader != nullptr)
	{
		--std::find_if(m_DomainShaderPtrMap.begin(), m_DomainShaderPtrMap.end(),
			[pDomainShader](const std::pair<size_t, std::pair<ID3D11DomainShader*, UINT>>& pair) { return pair.second.first == pDomainShader; }
		)->second.second;
	}

	const ID3D11GeometryShader* pGeometryShader = pEffect->pGeometryShader;
	if (pGeometryShader != nullptr)
	{
		--std::find_if(m_GeometryShaderPtrMap.begin(), m_GeometryShaderPtrMap.end(),
			[pGeometryShader](const std::pair<size_t, std::pair<ID3D11GeometryShader*, UINT>>& pair) { return pair.second.first == pGeometryShader; }
		)->second.second;
	}

	const ID3D11PixelShader* pPixelShader = pEffect->pPixelShader;
	--std::find_if(m_PixelShaderPtrMap.begin(), m_PixelShaderPtrMap.end(),
		[pPixelShader](const std::pair<size_t, std::pair<ID3D11PixelShader*, UINT>>& pair) { return pair.second.first == pPixelShader; }
	)->second.second;
}

void SpringWindEngine::ShaderStorage::DestroyUnusedShaders()
{
	for (std::unordered_map<size_t, std::pair<InputLayoutDesc*, UINT>>::const_iterator it = m_InputLayoutPtrMap.cbegin(); it != m_InputLayoutPtrMap.cend();)
	{
		if (it->second.second == 0)
		{
			it->second.first->pInputLayout->Release();
			delete it->second.first;
			it = m_InputLayoutPtrMap.erase(it);
		}
		else
		{
			++it;
		}

	}
	for (std::unordered_map<size_t, std::pair<ID3D11VertexShader*, UINT>>::const_iterator it = m_VertexShaderPtrMap.cbegin(); it != m_VertexShaderPtrMap.cend();)
	{
		if (it->second.second == 0)
		{
			it->second.first->Release();
			it = m_VertexShaderPtrMap.erase(it);
		}
		else
		{
			++it;
		}
	}
	for (std::unordered_map<size_t, std::pair<ID3D11HullShader*, UINT>>::const_iterator it = m_HullShaderPtrMap.cbegin(); it != m_HullShaderPtrMap.cend();)
	{
		if (it->second.second == 0)
		{
			it->second.first->Release();
			it = m_HullShaderPtrMap.erase(it);
		}
		else
		{
			++it;
		}
	}
	for (std::unordered_map<size_t, std::pair<ID3D11DomainShader*, UINT>>::const_iterator it = m_DomainShaderPtrMap.cbegin(); it != m_DomainShaderPtrMap.cend();)
	{
		if (it->second.second == 0)
		{
			it->second.first->Release();
			it = m_DomainShaderPtrMap.erase(it);
		}
		else
		{
			++it;
		}
	}
	for (std::unordered_map<size_t, std::pair<ID3D11GeometryShader*, UINT>>::const_iterator it = m_GeometryShaderPtrMap.cbegin(); it != m_GeometryShaderPtrMap.cend();)
	{
		if (it->second.second == 0)
		{
			it->second.first->Release();
			it = m_GeometryShaderPtrMap.erase(it);
		}
		else
		{
			++it;
		}
	}
	for (std::unordered_map<size_t, std::pair<ID3D11PixelShader*, UINT>>::const_iterator it = m_PixelShaderPtrMap.cbegin(); it != m_PixelShaderPtrMap.cend();)
	{
		if (it->second.second == 0)
		{
			it->second.first->Release();
			it = m_PixelShaderPtrMap.erase(it);
		}
		else
		{
			++it;
		}
	}
}