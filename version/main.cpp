#include <stdexcept>
#include <iostream>

#include "main.h"

void error_cb(int err, const char *descr) {
	std::cout << err << ": "<< descr << std::endl;
}

//WNDPROC prevWinProc;

//LRESULT CALLBACK winProc(HWND handle, UINT uMsg, WPARAM param, LPARAM lparam)
//{
//	switch (uMsg)
//	{
//	case WM_MOUSEMOVE:
//
//		std::cout << "Mouse Moving" << std::endl;
//	}
//
//	return CallWindowProcW(prevWinProc, handle, uMsg, param, lparam);
//}

HHOOK hhook{ nullptr };

LRESULT CALLBACK LLMouseHook(const int n_code, const WPARAM w_param, const LPARAM l_param)
{
	switch(w_param)
	{
	case WM_MOUSEMOVE:
		POINT p;
		GetCursorPos(&p);
		App::instance().systemHit(p.x, p.y);
		break;
	}

	return CallNextHookEx(hhook, n_code, w_param, l_param);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

App::~App()
{
	glfwDestroyWindow(r_window);
	glfwTerminate();
}

bool App::systemHit(const int x, const int y)
{
	return true;
	// hit test here
	if (x > 404 && x < 639 && y > 368 && y < 580)
	{
		SetWindowLongPtrW(rhandle, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TOPMOST); // to make it invisible to mouse events
		std::cout << "visible!" << std::endl;
	}
	else
	{
		SetWindowLongPtrW(rhandle, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST); // to make it invisible to mouse events
		std::cout << "Invisible!" << std::endl;
	}

	return true;
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
	r_window = glfwCreateWindow(window_size.x, window_size.y, "Mascot", nullptr, nullptr);

	if (!r_window)
		throw std::runtime_error("Could not create window");

	glfwSetKeyCallback(r_window, keyCallback);

	// Set window properties

	rhandle = glfwGetWin32Window(r_window);
	//prevWinProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(handle, GWL_WNDPROC, reinterpret_cast<LONG_PTR>(&winProc)));
	SetWindowLongPtrW(rhandle, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST); // to make it invisible to mouse events
	SetWindowPos(rhandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED); // to make it top most

	// set hook to monitor global mouse events
	hhook = SetWindowsHookEx(WH_MOUSE_LL, LLMouseHook, nullptr, 0);
	if (!hhook)
	{
		throw std::runtime_error("Mouse hook failed!");
	}

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
		-1.0f, -1.0f, -1.0f,   0.0f,  1.0f,  0.0f,
		1.0f, -1.0f, -1.0f,    0.0f,  1.0f,  0.0f,
		1.0f,  1.0f, -1.0f,    1.0f,  0.5f,  0.0f,
		-1.0f, 1.0f, -1.0f,    1.0f,  0.5f,  0.0f,
		-1.0f, -1.0f,  1.0f,   1.0f,  0.0f,  0.0f,
		1.0f, -1.0f,  1.0f,    1.0f,  0.0f,  0.0f,
		1.0f,  1.0f,  1.0f,    0.0f,  0.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,   1.0f,  0.0f,  1.0f
	};

	GLuint indices[] = {  // Note that we start from 0!
		0, 4, 5, 0, 5, 1,
		1, 5, 6, 1, 6, 2,
		2, 6, 7, 2, 7, 3,
		3, 7, 4, 3, 4, 0,
		4, 7, 6, 4, 6, 5,
		3, 0, 1, 3, 1, 2
	};
	
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glUseProgram(program->object());

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(0));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid*>(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1, 0, 0));
	view = glm::translate(view, glm::vec3(0, 0, -10));
	projection = glm::perspective(glm::radians(45.0f), window_size.x / window_size.y, 0.1f, 100.0f);

}

void App::render() const
{
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
}

void App::postRender()
{
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
		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		auto model_loc = glGetUniformLocation(program->object(), "model");
		model = glm::rotate(model, glm::radians(0.50f), glm::vec3(0.5f, 1.0f, 0.0f));
		glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
		auto view_loc = glGetUniformLocation(program->object(), "view");
		glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
		auto projection_loc = glGetUniformLocation(program->object(), "projection");
		glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection));

		//c_time = glfwGetTime();
		//red = (sin(c_time) / 2) + rand();
		//green = (sin(c_time) / 2) + rand();
		//blue = (sin(c_time) / 2) + rand();

		//glUniform3f(frag_color_loc, red, green, blue);
		glUseProgram(program->object());
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
		system("pause");
	}
	
	return 0;
}
