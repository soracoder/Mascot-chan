#pragma once

#include <vector>
#include <memory>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL/SOIL.h>

#include "shader.h"
#include "program.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>

LRESULT CALLBACK winProc(HWND handle, UINT uMsg, WPARAM param, LPARAM lparam);
LRESULT CALLBACK LLMouseHook(const int n_code, const WPARAM w_param, const LPARAM l_param);

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

	GLuint VBO;
	GLuint VAO;
	GLuint EBO;
	GLuint texture[2];

	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 projection;

private:
	virtual ~App();
	bool systemHit(const int x, const int y);

	static App *inst;
	
	GLFWwindow *r_window;
	HWND rhandle;

	glm::vec2 window_size{ 1040, 780 };

	bool inited = false;
	bool system_visible = false; // model hittest

	std::vector<Shader> shaders;
	std::shared_ptr<Program> program;

	friend LRESULT CALLBACK winProc(HWND handle, UINT uMsg, WPARAM param, LPARAM lparam);
	friend LRESULT CALLBACK LLMouseHook(const int n_code, const WPARAM w_param, const LPARAM l_param);
};

App* App::inst = nullptr;