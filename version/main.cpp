#include <stdexcept>
#include <iostream>

#include "main.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>

void error_cb(int err, const char *descr) {
	std::cout << err << ": "<< descr << std::endl;
}

App::~App()
{
	glfwDestroyWindow(r_window);
	glfwTerminate();
}

int App::init()
{
	glfwSetErrorCallback(error_cb);
	if (!glfwInit()) {
		throw std::runtime_error("Could not init glfw");
	}
	glEnable(GL_MULTISAMPLE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DEPTH_BITS, 16);
	glfwWindowHint(GLFW_ALPHA_BITS, 8);
	glfwWindowHint(GLFW_TRANSPARENT, true);
	glfwWindowHint(GLFW_DECORATED, false);
	glfwWindowHint(GLFW_SAMPLES, 12);
	r_window = glfwCreateWindow(windowSize.x, windowSize.y, "Mascot", nullptr, nullptr);

	if (!r_window)
		throw std::runtime_error("Could not create window");

	auto handle = glfwGetWin32Window(r_window);
	SetWindowLongPtrW(handle, GWL_EXSTYLE, WS_EX_LAYERED);
	if (!SetLayeredWindowAttributes(handle, RGB(0, 0, 0), 50, LWA_COLORKEY))
		throw std::runtime_error("Could not make alpha");
	glfwMakeContextCurrent(r_window);

	glewExperimental = GL_TRUE;
	auto glew_result = glewInit();
	if (glew_result != GLEW_OK) {
		throw std::runtime_error("Could not init glew");
	}
	// GLEW generates GL error because it calls glGetString(GL_EXTENSIONS), we'll consume it here.
	glGetError();

	inited = true;

	return 0;
}

void App::render() const
{
}

void App::exec() const
{
	while(!glfwWindowShouldClose(r_window))
	{
		int fb_width;
		int fb_height;
		glfwGetFramebufferSize(r_window, &fb_width, &fb_height);
		glViewport(0, 0, fb_width, fb_height);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		render();
		glfwSwapBuffers(r_window);
		glfwPollEvents();
	}
}


int main()
{

	App &app = App::instance();

	try
	{
		app.init();
		app.exec();

	} catch(const std::exception &e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
	}
	
	return 0;
}
