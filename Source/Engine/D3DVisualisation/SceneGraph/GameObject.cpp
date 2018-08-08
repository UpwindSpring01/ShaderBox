#include "stdafx.h"

#include "GameObject.h"
#include "../Component/TransformComponent.h"
#include "../Component/BaseComponent.h"

GameObject::GameObject()
{
	m_pTransComp = new TransformComponent();
	AddComponent(m_pTransComp);
}

GameObject::~GameObject()
{
	for(BaseComponent* pComp : m_pBaseCompArr)
	{
		delete pComp;
	}
}

void GameObject::RootDraw(const GameContext& gameContext)
{
	for(BaseComponent* pComp : m_pBaseCompArr)
	{
		pComp->Draw(gameContext);
	}
}

void GameObject::RootPostDraw()
{
	for(BaseComponent* pComp : m_pBaseCompArr)
	{
		pComp->PostDraw();
	}
}

void GameObject::RootUpdate()
{
	for(BaseComponent* pComp : m_pBaseCompArr)
	{
		pComp->Update();
	}
}

void GameObject::AddComponent(BaseComponent* pComp)
{
#ifdef _DEBUG
	if(dynamic_cast<Component<TransformComponent>*>(pComp) != nullptr && m_pBaseCompArr.size() != 0)
	{
		Logger::LogError(L"Can not add TransformComponent manually");
	}
	else
	{
		for(BaseComponent* pAddedComp : m_pBaseCompArr)
		{
			if(pComp == pAddedComp)
			{
				Logger::LogError(L"Component already added");
			}
			else if(typeid(pComp) == typeid(pAddedComp) && !pComp->IsMultipleComponentAllowed())
			{
				Logger::LogError(L"Component type already added, multiple component is not allowed for this component");
			}
		}
	}
#endif // _DEBUG
	pComp->m_pGameObject = this;
	m_pBaseCompArr.emplace_back(pComp);
}