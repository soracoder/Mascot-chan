#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
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


	int init();

	void render() const;

	void exec() const;

	static App& instance()
	{
		static App tinst;
		inst = &tinst;
		return *inst;
	}

private:
	virtual ~App();

	static App *inst;
	
	GLFWwindow *r_window;

	glm::vec2 windowSize{ 1040, 780 };

	bool inited = false;

};

App* App::inst = nullptr;