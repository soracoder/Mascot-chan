#pragma once

#include <irrlicht/irrlicht.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

class App
{
public:
	App() = default;

	App(const App& other)
	{
		inst = other.inst;
	}

	App& operator=(const App& other) {
		if (this != &other)
			inst = other.inst;
		return *this;
	}


	int init(HWND& handle);

	void createNode();

	void render() const;

	void advance() const;

	static App& instance()
	{
		static App tinst;
		inst = &tinst;
		return *inst;
	}

private:
	virtual ~App();

	static App *inst;
	irr::IrrlichtDevice *device;
	irr::video::IVideoDriver *driver;
	irr::scene::ISceneManager *scene;
	irr::gui::IGUIEnvironment *guienv;

	glm::vec2 windowSize{ 1040, 780 };

	bool inited = false;

	irr::scene::IAnimatedMeshSceneNode *node;


};

App* App::inst = nullptr;