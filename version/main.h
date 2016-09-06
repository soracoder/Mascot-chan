#pragma once
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "shader.h"
#include "program.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>

LRESULT CALLBACK winProc(HWND handle, UINT uMsg, WPARAM param, LPARAM lparam);

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

	bool hittest();

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

	friend LRESULT CALLBACK winProc(HWND handle, UINT uMsg, WPARAM param, LPARAM lparam);
};

App* App::inst = nullptr;