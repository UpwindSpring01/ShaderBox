#pragma once

namespace SpringWindEngine
{
	class GameObject;

	class BaseComponent
	{
	public:
		BaseComponent() {};
		virtual ~BaseComponent() {};

		GameObject* GetGameObject() const { return m_pGameObject; }

#ifdef _DEBUG
		virtual bool IsMultipleComponentAllowed() const { return true; }
#endif _DEBUG
	protected:
		friend class GameObject;
		GameObject* m_pGameObject = nullptr;
	private:
		friend class GameScene;
		virtual void Update() = 0;
		virtual void Draw(const EngineContext& engineContext) = 0;
		virtual void SetTransformDirty() {};


		BaseComponent(const BaseComponent& t) = delete;
		BaseComponent& operator=(const BaseComponent& t) = delete;
	};
}