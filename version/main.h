#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "shader.h"
#include "program.h"

class App
{
public:
	App(){};

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

	void exec();

	static App& instance()
	{
		static App tinst;
		inst = &tinst;
		return *inst;
	}

protected:
	void preRender();
	void render() const;
	void postRender();

	void hittest(glm::vec2 mouse_pos);

	GLuint VBO;
	GLuint VAO;
	GLuint EBO;

private:
	virtual ~App();

	static App *inst;
	
	GLFWwindow *r_window;

	glm::vec2 windowSize{ 1040, 780 };

	bool inited = false;


	std::vector<Shader> shaders;
	std::shared_ptr<Program> program;
};

App* App::inst = nullptr;