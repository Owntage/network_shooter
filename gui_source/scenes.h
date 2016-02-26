#ifndef SCENES_H
#define SCENES_H

#include <memory>
#include <stack>
#include "singleton.h"

struct WindowEvent;

struct Scene
{

	virtual void onFrame() = 0;
	virtual void onEvent(WindowEvent&) = 0;
	virtual void onDestroy() {}
	
protected:

	Scene() {}
	virtual ~Scene() {}
	Scene(const Scene&) {}
	Scene& operator=(const Scene&) {}
};

struct SceneManager: public Singleton <SceneManager>
{
	SceneManager() {}
	void onEvent(WindowEvent&);
	void onFrame();
	void startScene(std::shared_ptr<Scene>);
	void closeScene();
	bool isEmpty() { return scenes.size() == 0; }
private:
	SceneManager(const SceneManager&);
	SceneManager& operator=(const SceneManager&);
	std::stack<std::shared_ptr<Scene>> scenes;
	
};

#endif