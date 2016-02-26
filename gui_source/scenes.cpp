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
		scenes.top()->onDestroy();
		scenes.pop();
	}
}

