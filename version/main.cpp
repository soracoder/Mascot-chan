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

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
	glfwMakeContextCurrent(r_window);

	glewExperimental = GL_TRUE;
	auto glew_result = glewInit();
	if (glew_result != GLEW_OK) {
		throw std::runtime_error("Could not init glew");
	}
	// GLEW generates GL error because it calls glGetString(GL_EXTENSIONS), we'll consume it here.
	glGetError();

	// initate program & shaders
	shaders = { Shader::shaderFromFile("data/shaders/vertex.shader", GL_VERTEX_SHADER), Shader::shaderFromFile("data/shaders/fragment.shader", GL_FRAGMENT_SHADER) };
	program = std::make_shared<Program>(shaders);

	inited = true;

	return 0;
}

void App::preRender()
{
	int fb_width, fb_height;
	glfwGetFramebufferSize(r_window, &fb_width, &fb_height);
	glViewport(0, 0, fb_width, fb_height);

	GLfloat vertices[] = {
		// Positions         // Colors
		0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // Bottom Right
		-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // Bottom Left
		0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // Top 
	};

	//GLuint indices[] = {  // Note that we start from 0!
	//	0, 1, 3,   // First Triangle
	//	1, 2, 3    // Second Triangle
	//};
	
	glGenBuffers(1, &VBO);
	//glGenBuffers(1, &EBO);


	glUseProgram(program->object());

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

}

void App::render() const
{
	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void App::postRender()
{
}

void App::hittest(glm::vec2 mouse_pos)
{
	auto handle = glfwGetWin32Window(r_window);

}

void App::exec()
{
	preRender();
	//GLfloat c_time, red, green, blue;
	//GLint frag_color_loc = glGetUniformLocation(program->object(), "delta_color");

	while(!glfwWindowShouldClose(r_window))
	{

		glfwPollEvents();
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUseProgram(program->object());

		//c_time = glfwGetTime();
		//red = (sin(c_time) / 2) + rand();
		//green = (sin(c_time) / 2) + rand();
		//blue = (sin(c_time) / 2) + rand();

		//glUniform3f(frag_color_loc, red, green, blue);

		glBindVertexArray(VAO);
		render();
		glBindVertexArray(0);
		glfwSwapBuffers(r_window);
	}
	postRender();
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
