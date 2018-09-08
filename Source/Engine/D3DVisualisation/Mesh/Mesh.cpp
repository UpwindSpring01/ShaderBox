#include "stdafx.h"

#include "Mesh.h"

XMFLOAT4 SpringWindEngine::Mesh::m_DefaultColor = XMFLOAT4(1, 0, 0, 1);
XMFLOAT4 SpringWindEngine::Mesh::m_DefaultFloat4 = XMFLOAT4(0, 0, 0, 0);
XMFLOAT3 SpringWindEngine::Mesh::m_DefaultFloat3 = XMFLOAT3(0, 0, 0);
XMFLOAT2 SpringWindEngine::Mesh::m_DefaultFloat2 = XMFLOAT2(0, 0);

SpringWindEngine::Mesh::Mesh(std::vector<WaveFrontReader<unsigned int>::Vertex> vertices,
	std::vector<unsigned int> indices, UINT elements)
	: m_Vertices(vertices),
	m_Indices(indices),
	m_HasElement(elements)
{
}

SpringWindEngine::Mesh::~Mesh()
{
}

SpringWindEngine::Unsafe_Shared_Ptr<ID3D11Buffer> SpringWindEngine::Mesh::GetVertexBuffer(InputLayoutDesc* pInputLayoutDesc)
{
	std::unordered_map<InputLayoutDesc*, Unsafe_Shared_Ptr<ID3D11Buffer>>::const_iterator it = m_VertexBufferPtrMap.find(pInputLayoutDesc);
	if (it != m_VertexBufferPtrMap.cend())
	{
		return (*it).second;
	}

	size_t vertexCount = m_Vertices.size();

	size_t bufferSize = pInputLayoutDesc->InputLayoutSize * vertexCount;
	char* pData = new char[bufferSize];
	char* pDataStart = pData;

	for (UINT i = 0; i < vertexCount; ++i)
	{
		for (const ILDescription& description : pInputLayoutDesc->InputLayoutDescriptions)
		{
			switch (description.SemanticType)
			{
			case ILSemantic::POSITION:
				memcpy(pData, HasElement(description.SemanticType) ? &m_Vertices[i].position : &m_DefaultFloat3, description.Offset);
				break;
			case ILSemantic::NORMAL:
				memcpy(pData, HasElement(description.SemanticType) ? &m_Vertices[i].normal : &m_DefaultFloat3, description.Offset);
				break;
			case ILSemantic::COLOR:
				memcpy(pData, HasElement(description.SemanticType) ? &m_DefaultColor : &m_DefaultColor, description.Offset);
				break;
			case ILSemantic::TEXCOORD:
				memcpy(pData, HasElement(description.SemanticType) ? &m_Vertices[i].textureCoordinate : &m_DefaultFloat2, description.Offset);
				break;
			case ILSemantic::TANGENT:
				memcpy(pData, HasElement(description.SemanticType) ? &m_DefaultFloat3 : &m_DefaultFloat3, description.Offset);
				break;
			case ILSemantic::BINORMAL:
				memcpy(pData, HasElement(description.SemanticType) ? &m_DefaultFloat3 : &m_DefaultFloat3, description.Offset);
				break;
			default:
#ifdef _DEBUG
				Logger::LogError(L"MeshFilter::BuildVertexBuffer() > Unsupported SemanticType!");
#endif // _DEBUG


				break;
			}

			pData = pData + description.Offset;
		}
	}

	//fill a buffer description to copy the vertexdata into graphics memory
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
	bd.CPUAccessFlags = 0;
	bd.ByteWidth = (UINT)bufferSize;
	bd.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = pDataStart;

	//create a ID3D11Buffer in graphics memory containing the vertex info
	ID3D11Buffer* pBuffer = nullptr;
	CHECK_HR(SpringWind::GetInstance()->GetEngineContext().pDevice->CreateBuffer(&bd, &initData, &pBuffer));
	delete[] pDataStart;
	
	Unsafe_Shared_Ptr<ID3D11Buffer> pSharedBuffer(pBuffer);
	m_VertexBufferPtrMap.emplace(pInputLayoutDesc, pSharedBuffer);

	return pSharedBuffer;
}

SpringWindEngine::Unsafe_Shared_Ptr<std::pair<ID3D11Buffer*, UINT>> SpringWindEngine::Mesh::GetIndexBuffer(D3D11_PRIMITIVE_TOPOLOGY topology)
{
	std::unordered_map<D3D11_PRIMITIVE_TOPOLOGY, Unsafe_Shared_Ptr<std::pair<ID3D11Buffer*, UINT>>>::iterator it = m_IndexBufferPtrMap.find(topology);
	if (it != m_IndexBufferPtrMap.end())
	{
		return (*it).second;
	}

	uint32_t* indices;
	UINT indicesCount;
	if (topology == D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ)
	{
		size_t indicesSize = m_Indices.size();
		size_t nFaces = indicesSize / 3;
		std::vector<XMFLOAT3> positions;
		size_t nVerts = m_Vertices.size();
		positions.reserve(nVerts);
		for (const auto& vertex : m_Vertices)
		{
			positions.emplace_back(vertex.position);
		}

		uint32_t* adj = new uint32_t[indicesSize];
		uint32_t* pointReps = new uint32_t[nVerts];
		uint32_t* indicesAdj = new uint32_t[indicesSize * 2];

		GenerateAdjacencyAndPointReps(m_Indices.data(), nFaces, positions.data(), nVerts, 0.f, pointReps, adj);
		GenerateGSAdjacency(m_Indices.data(), nFaces, pointReps, adj, nVerts, indicesAdj);

		delete[] adj;
		delete[] pointReps;

		indices = indicesAdj;
		indicesCount = static_cast<UINT>(indicesSize * 2);
	}
	else
	{
		indices = m_Indices.data();
		indicesCount = static_cast<UINT>(m_Indices.size());
	}

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = (UINT)(sizeof(uint32_t) * indicesCount);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = indices;

	ID3D11Buffer* pBuffer;
	SpringWind::GetInstance()->GetEngineContext().pDevice->CreateBuffer(&bd, &initData, &pBuffer);

	if (topology == D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ)
	{
		delete[] indices;
	}

	return m_IndexBufferPtrMap.emplace(topology, Unsafe_Make_Shared_Pair<ID3D11Buffer, UINT>(pBuffer, indicesCount)).first->second;
}

void SpringWindEngine::Mesh::UnloadUnusedBuffers()
{
	for (std::unordered_map<InputLayoutDesc*, Unsafe_Shared_Ptr<ID3D11Buffer>>::const_iterator it = m_VertexBufferPtrMap.cbegin(); it != m_VertexBufferPtrMap.cend();)
	{
		if (it->second.UseCount() == 1)
		{
			it = m_VertexBufferPtrMap.erase(it);
		}
		else
		{
			++it;
		}
	}

	for (std::unordered_map<D3D11_PRIMITIVE_TOPOLOGY, Unsafe_Shared_Ptr<std::pair<ID3D11Buffer*, UINT>>>::const_iterator it = m_IndexBufferPtrMap.cbegin(); it != m_IndexBufferPtrMap.cend();)
	{
		if (it->second.UseCount() == 1)
		{
			it = m_IndexBufferPtrMap.erase(it);
		}
		else
		{
			++it;
		}
	}
}
