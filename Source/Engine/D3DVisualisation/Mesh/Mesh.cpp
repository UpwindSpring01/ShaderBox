#include "stdafx.h"

#include "Mesh.h"

XMFLOAT4 Mesh::m_DefaultColor = XMFLOAT4(1, 0, 0, 1);
XMFLOAT4 Mesh::m_DefaultFloat4 = XMFLOAT4(0, 0, 0, 0);
XMFLOAT3 Mesh::m_DefaultFloat3 = XMFLOAT3(0, 0, 0);
XMFLOAT2 Mesh::m_DefaultFloat2 = XMFLOAT2(0, 0);

Mesh::Mesh(std::vector<WaveFrontReader<unsigned int>::Vertex> vertices, std::vector<unsigned int> indices, UINT elements)
	: m_Vertices(vertices),
	m_Indices(indices),
	m_HasElement(elements)
{
}

Mesh::~Mesh()
{
	SafeRelease(m_pVertexBuffer);
	SafeRelease(m_pIndexBuffer);
}

void Mesh::CreateVertexBuffer()
{
	size_t vertexCount = m_Vertices.size();
	size_t bufferSize = m_pEffect->m_pInputLayoutSize * vertexCount;
	char* pData = new char[bufferSize];
	char* pDataStart = pData;
	for (UINT i = 0; i < vertexCount; ++i)
	{
		for (UINT j = 0; j < m_pEffect->m_pInputLayoutDescriptions.size(); ++j)
		{
			auto ilDescription = m_pEffect->m_pInputLayoutDescriptions[j];

			switch (ilDescription.SemanticType)
			{
			case ILSemantic::POSITION:
				memcpy(pData, HasElement(ilDescription.SemanticType) ? &m_Vertices[i].position : &m_DefaultFloat3, ilDescription.Offset);
				break;
			case ILSemantic::NORMAL:
				memcpy(pData, HasElement(ilDescription.SemanticType) ? &m_Vertices[i].normal : &m_DefaultFloat3, ilDescription.Offset);
				break;
			case ILSemantic::COLOR:
				memcpy(pData, HasElement(ilDescription.SemanticType) ? &m_DefaultColor : &m_DefaultColor, ilDescription.Offset);
				break;
			case ILSemantic::TEXCOORD:
				memcpy(pData, HasElement(ilDescription.SemanticType) ? &m_Vertices[i].textureCoordinate : &m_DefaultFloat2, ilDescription.Offset);
				break;
			case ILSemantic::TANGENT:
				memcpy(pData, HasElement(ilDescription.SemanticType) ? &m_DefaultFloat3 : &m_DefaultFloat3, ilDescription.Offset);
				break;
			case ILSemantic::BINORMAL:
				memcpy(pData, HasElement(ilDescription.SemanticType) ? &m_DefaultFloat3 : &m_DefaultFloat3, ilDescription.Offset);
				break;
			default:
#ifdef _DEBUG
				Logger::LogError(L"MeshFilter::BuildVertexBuffer() > Unsupported SemanticType!");
#endif // _DEBUG

				
				break;
			}

			pData = pData + ilDescription.Offset;
		}
	}

	//fill a buffer description to copy the vertexdata into graphics memory
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0;
	bd.ByteWidth = (UINT)bufferSize;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = pDataStart;
	//create a ID3D11Buffer in graphics memory containing the vertex info
	SpringWind::GetInstance()->GetGameContext().pDevice
		->CreateBuffer(&bd, &initData, &m_pVertexBuffer);

	delete[] pDataStart;
}

void Mesh::CreateIndexBuffer(uint32_t* indices, size_t count)
{
	m_IndexCount = (UINT)count;

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = (UINT)(sizeof(uint32_t) * count);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = indices;

	SpringWind::GetInstance()->GetGameContext().pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);

}

void Mesh::SwapData(std::vector<WaveFrontReader<unsigned int>::Vertex> vertices, std::vector<unsigned int> indices, UINT elements)
{
	m_HasElement = elements;
	m_Vertices = vertices;
	m_Indices = indices;

	if (m_pEffect != nullptr)
	{
		UpdateBuffers();

	}
}

void Mesh::SetEffect(Effect* effect)
{
	m_pEffect = effect;

	UpdateBuffers();
}

void Mesh::UpdateBuffers()
{
	SafeRelease(m_pIndexBuffer);
	SafeRelease(m_pVertexBuffer);
	if (m_pEffect->m_Topology == D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ)
	{
		size_t indicesSize = m_Indices.size();
		size_t nFaces = indicesSize / 3;
		std::vector<XMFLOAT3> positions;
		size_t nVerts = m_Vertices.size();
		positions.reserve(nVerts);
		for(const auto& vertex : m_Vertices)
		{
			positions.emplace_back(vertex.position);
		}

		std::unique_ptr<uint32_t[]> adj(new uint32_t[indicesSize]);
		std::unique_ptr<uint32_t[]> pointReps(new uint32_t[nVerts]);
		std::unique_ptr<uint32_t[]> indicesAdj(new uint32_t[indicesSize * 2]);

		GenerateAdjacencyAndPointReps(m_Indices.data(), nFaces, positions.data(), nVerts, 0.f, pointReps.get(), adj.get());
		GenerateGSAdjacency(m_Indices.data(), nFaces, pointReps.get(), adj.get(), nVerts, indicesAdj.get());

		CreateIndexBuffer(indicesAdj.get(), indicesSize * 2);
	}
	else
	{
		CreateIndexBuffer(m_Indices.data(), m_Indices.size());
	}

	CreateVertexBuffer();
}

void Mesh::Draw() const
{
	ID3D11DeviceContext* pDeviceContext = SpringWind::GetInstance()->GetGameContext().pDeviceContext;
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &m_pEffect->m_pInputLayoutSize, &offset);
	pDeviceContext->IASetPrimitiveTopology(m_pEffect->m_Topology);

	pDeviceContext->IASetInputLayout(m_pEffect->InputLayoutPtr);
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->DrawIndexed(m_IndexCount, 0, 0);
}