#pragma once

namespace SpringWindEngine
{
	class GameObject;
	class SpringWindWindow;

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
		void AddChild(GameObject* pObj);
		void Draw(const EngineContext& gameContext, const WindowContext& windowContext, const RenderTarget& mainRT, const RenderTarget& ppRT);

		const SpringWindWindow* GetWindow() const { return m_pWindow; }
		CameraComponent* GetActiveCamera() const { return m_pActiveCamera; }
		const std::vector<GameObject*>& GetChildren() const { return m_ChildPtrArr; }

		void SetActiveCamera(CameraComponent* pComponent);
	private:
		void RootUpdate();

		friend class SceneManager;
		friend class SpringWindWindow;
		GameScene();
		virtual ~GameScene();

		void SetWindow(SpringWindWindow* pWindow) { m_pWindow = pWindow; }

		ID3D11Buffer* m_pVertexBuffer = nullptr;

		CameraComponent* m_pActiveCamera = nullptr;

		std::vector<GameObject*> m_ChildPtrArr;

		SpringWindWindow* m_pWindow = nullptr;

		GameScene(const GameScene &obj) = delete;
		GameScene& operator=(const GameScene& obj) = delete;
	};
}