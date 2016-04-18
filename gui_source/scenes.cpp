#include "scenes.h"

void SceneManager::onEvent(WindowEvent& windowEvent)
{
	if(scenes.size() > 0)
	{
		scenes.top()->onEvent(windowEvent);
	}
}

void SceneManager::onFrame()
{
	if(scenes.size() > 0)
	{
		scenes.top()->onFrame();
		if(scenes.top()->destroyFlag)
		{
			scenes.pop();
		}
	}
}

void SceneManager::startScene(std::shared_ptr<Scene> newScene)
{
	scenes.push(newScene);
}

void SceneManager::closeScene()
{
	if(scenes.size() > 0)
	{
		//scenes.top()->onDestroy();
		//scenes.pop();
		scenes.top()->destroyFlag = true;
	}
}

