#pragma once

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
	virtual void Draw(const GameContext& gameContext) = 0;
	virtual void PostDraw() = 0;
	

	BaseComponent(const BaseComponent& t) = delete;
	BaseComponent& operator=(const BaseComponent& t) = delete;
};