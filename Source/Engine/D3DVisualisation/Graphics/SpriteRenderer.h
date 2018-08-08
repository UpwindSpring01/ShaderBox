#pragma once
#include "../Helpers/Singleton.h"

class TextureData;
class OverlordGame;
class GameScene;
struct Effect;

class SpriteRenderer: public Singleton<SpriteRenderer>
{
public:

	void DrawImmediate(const GameContext& gameContext, ID3D11ShaderResourceView* pSrv, XMFLOAT2 position, XMFLOAT4 color, XMFLOAT2 pivot, XMFLOAT2 scale, float rotation);
private:
	friend class GameScene;
	friend class Singleton<SpriteRenderer>;

	SpriteRenderer();
	~SpriteRenderer();

	ID3D11Buffer* m_pImmediateVertexBuffer;
	struct SpriteVertex
	{
		UINT TextureId;
		XMFLOAT4 TransformData;
		XMFLOAT4 TransformData2;
		XMFLOAT4 Color;

		bool Equals(SpriteVertex vertex)
		{
			return TextureId == vertex.TextureId
				&& XMFloat4Equals(TransformData, vertex.TransformData)
				&& XMFloat4Equals(TransformData2, vertex.TransformData2)
				&& XMFloat4Equals(Color, vertex.Color);
		}
	};
private:
	SpriteVertex m_ImmediateVertex;
	Effect* m_pEffect = nullptr;
	XMFLOAT4X4 m_Transform;
	ID3D11Buffer* m_pTextureSizeBuffer = nullptr;
	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	SpriteRenderer(const SpriteRenderer& t) = delete;
	SpriteRenderer& operator=(const SpriteRenderer& t) = delete;
};

