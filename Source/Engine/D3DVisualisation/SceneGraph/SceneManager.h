#pragma once

#include "Helpers/Singleton.h"

namespace SpringWindEngine
{
	class SceneManager sealed : public Singleton<SceneManager>
	{
	public:
		~SceneManager();

		GameScene* CreateScene();
		void DestroyScene(GameScene* pScene);

		const std::unordered_set<GameScene*>& GetScenes() const { return m_ScenePtrSet; }
	private:
		std::unordered_set<GameScene*> m_ScenePtrSet;
	};
}