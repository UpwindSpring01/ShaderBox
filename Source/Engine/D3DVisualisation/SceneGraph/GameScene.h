#pragma once

class GameObject;

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

struct VertexPosTex
{
public:

	VertexPosTex() {};
	VertexPosTex(XMFLOAT3 pos, XMFLOAT2 uv) :
		Position(pos), UV(uv) {}

	XMFLOAT3 Position;
	XMFLOAT2 UV;
};

struct QuadPosTex
{
public:

	QuadPosTex() {};
	QuadPosTex(VertexPosTex vert1, VertexPosTex vert2, VertexPosTex vert3, VertexPosTex vert4) :
		Vertex1(vert1), Vertex2(vert2), Vertex3(vert3), Vertex4(vert4) {}

	VertexPosTex Vertex1;
	VertexPosTex Vertex2;
	VertexPosTex Vertex3;
	VertexPosTex Vertex4;
};



class GameScene
{
public:
	GameScene(const GameContext& gameContext);
	virtual ~GameScene();

	void AddChild(GameObject* pObj);
	void Draw(const GameContext& gameContext);
	void UpdateObjectBuffer(const GameContext& gameContext);
	void BindBuffers();
	BaseConstantBufferObject* GetObjectDataCBuffer() const { return m_pBaseCBufferObjectData; }

	void ClearPPShader()
	{
		m_pPPEffect = nullptr;
	};
	void AddPPShader(Effect* pEffect);
private:
	void RootUpdate();
	void CreateVertexBuffer();
private:
	void InitFrameAndObjectBuffers();
	void InitSamplers();

	ID3D11Buffer *m_pBaseCBufferFrameBuffer;
	BaseConstantBufferFrame *m_pBaseCBufferFrameData;

	ID3D11Buffer *m_pBaseCBufferObjectBuffer;
	BaseConstantBufferObject *m_pBaseCBufferObjectData;

	static constexpr UINT SAMPLER_AMOUNT = 6;
	ID3D11SamplerState *m_pSamplersArr[SAMPLER_AMOUNT];

	Effect* m_pPPEffect = nullptr;
	ID3D11Buffer* m_pVertexBuffer = nullptr;

	GameContext m_GameContext;
	std::vector<GameObject*> m_ChildArr;

	GameScene(const GameScene &obj) = delete;
	GameScene& operator=(const GameScene& obj) = delete;
};