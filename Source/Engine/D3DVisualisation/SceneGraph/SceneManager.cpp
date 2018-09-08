#include "stdafx.h"

#include "SceneManager.h"

SpringWindEngine::SceneManager::~SceneManager()
{
	for (GameScene* pScene : m_ScenePtrSet)
	{
		delete pScene;
	}
}

SpringWindEngine::GameScene* SpringWindEngine::SceneManager::CreateScene()
{
	GameScene* pScene = new GameScene();
	m_ScenePtrSet.insert(pScene);

	return pScene;
}

void SpringWindEngine::SceneManager::DestroyScene(GameScene* pScene)
{
	m_ScenePtrSet.erase(pScene);
	delete pScene;
}