#pragma once

struct vertex;

class Mesh
{
public:
	Mesh(std::vector<WaveFrontReader<unsigned int>::Vertex> vertices, std::vector<unsigned int> indices, UINT elements);
	virtual ~Mesh();

	void SwapData(std::vector<WaveFrontReader<unsigned int>::Vertex> vertices, std::vector<unsigned int> indices, UINT elements);
	void SetEffect(Effect* effect);
	void UpdateBuffers();
	void Draw() const;
private:
	bool HasElement(ILSemantic element) { return (m_HasElement&element) > 0 ? true : false; }

	void CreateVertexBuffer();
	void CreateIndexBuffer(uint32_t* indices, size_t count);

	std::vector<WaveFrontReader<unsigned int>::Vertex> m_Vertices;
	std::vector<unsigned int> m_Indices;
	UINT m_IndexCount = 0;

	// For making it easier to swap models, also keep inside mesh class reference to effect,
	// Should be only part of MeshDrawComponent its material
	Effect* m_pEffect = nullptr;

	ID3D11Buffer* m_pVertexBuffer = nullptr;
	ID3D11Buffer* m_pIndexBuffer = nullptr;
	UINT m_HasElement = 0;
	
	static XMFLOAT4 m_DefaultColor;
	static XMFLOAT4 m_DefaultFloat4;
	static XMFLOAT3 m_DefaultFloat3;
	static XMFLOAT2 m_DefaultFloat2;
};