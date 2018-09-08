#pragma once

namespace SpringWindEngine
{
	struct vertex;

	class Mesh
	{
	public:
		Mesh(std::vector<WaveFrontReader<unsigned int>::Vertex> vertices, std::vector<unsigned int> indices, UINT elements);
		virtual ~Mesh();

		Unsafe_Shared_Ptr<ID3D11Buffer> GetVertexBuffer(InputLayoutDesc* pInputLayoutDesc);
		Unsafe_Shared_Ptr<std::pair<ID3D11Buffer*, UINT>> GetIndexBuffer(D3D11_PRIMITIVE_TOPOLOGY topology);

		void UnloadUnusedBuffers();
	private:
		bool HasElement(ILSemantic element) { return (m_HasElement & element) != 0; }

		std::vector<WaveFrontReader<unsigned int>::Vertex> m_Vertices;
		std::vector<unsigned int> m_Indices;

		std::unordered_map<InputLayoutDesc*, Unsafe_Shared_Ptr<ID3D11Buffer>> m_VertexBufferPtrMap;
		std::unordered_map<D3D11_PRIMITIVE_TOPOLOGY, Unsafe_Shared_Ptr<std::pair<ID3D11Buffer*, UINT>>> m_IndexBufferPtrMap;

		UINT m_HasElement = 0;

		static XMFLOAT4 m_DefaultColor;
		static XMFLOAT4 m_DefaultFloat4;
		static XMFLOAT3 m_DefaultFloat3;
		static XMFLOAT2 m_DefaultFloat2;
	};
}