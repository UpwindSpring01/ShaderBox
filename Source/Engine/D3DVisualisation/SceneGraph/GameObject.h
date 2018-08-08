#pragma once

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
private:
	friend class GameScene;
	friend class TransformComponent;

	void RootDraw(const GameContext& gameContext);
	void RootPostDraw();
	void RootUpdate();

	friend class GameScene;
	GameScene* m_pScene = nullptr;

	TransformComponent* m_pTransComp = nullptr;
	std::vector<BaseComponent*> m_pBaseCompArr;
};