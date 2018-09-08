#pragma once

namespace SpringWindEngine
{
	class BaseComponent;
	class TransformComponent;

	class GameObject
	{
	public:
		GameObject();
		virtual ~GameObject();

		void AddComponent(BaseComponent* pComp);
		GameScene* GetScene() const { return m_pScene; }

		TransformComponent* GetTransform() const { return m_pTransComp; }
		const std::vector<BaseComponent*>& GetComponents() const { return m_pBaseCompArr; }
	private:
		friend class GameScene;
		friend class TransformComponent;

		void RootDraw(const EngineContext& engineContext);
		void RootUpdate();
		void SetTransformDirty();

		friend class GameScene;
		GameScene* m_pScene = nullptr;
		enum DirtyFlag : byte
		{
			SW_DIRTY_NONE = 0,
			SW_DIRTY_TRANSFORM = 1 << 1,
		};

		byte m_DirtyFlag = SW_DIRTY_NONE;

		TransformComponent* m_pTransComp = nullptr;
		std::vector<BaseComponent*> m_pBaseCompArr;
	};
}