#include <stdexcept>
#include <iostream>
#include <dwmapi.h>

#include "main.h"

using namespace irr::core;

// +---------+
// | Globals |
// +---------+
WCHAR                   *g_wcpAppName = L"Mascot";
INT                     g_iWidth = 256;
INT                     g_iHeight = 256;
MARGINS                 g_mgDWMMargins = { -1, -1, -1, -1 };


App::~App()
{
	device->closeDevice();
	device->drop();
}

int App::init(HWND &handle)
{
	irr::video::SExposedVideoData videodata(handle);
	irr::SIrrlichtCreationParameters param;
	param.DriverType = irr::video::EDT_DIRECT3D9;
	param.WindowId = reinterpret_cast<void*>(handle);

	device = irr::createDeviceEx(param);

	if (!device)
		std::runtime_error("Could not create device!");

	driver = device->getVideoDriver();

	if (param.DriverType == irr::video::EDT_OPENGL)
	{
		HDC HDc = GetDC(handle);
		PIXELFORMATDESCRIPTOR pfd = { 0 };
		pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		int pf = GetPixelFormat(HDc);
		DescribePixelFormat(HDc, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
		pfd.dwFlags |= PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
		pfd.cDepthBits = 16;
		pf = ChoosePixelFormat(HDc, &pfd);
		SetPixelFormat(HDc, pf, &pfd);
		videodata.OpenGLWin32.HDc = HDc;
		videodata.OpenGLWin32.HRc = wglCreateContext(HDc);
		wglShareLists((HGLRC)driver->getExposedVideoData().OpenGLWin32.HRc, (HGLRC)videodata.OpenGLWin32.HRc);
	}

	scene = device->getSceneManager();
	auto sceneparams = scene->getParameters();
	sceneparams->setAttribute(irr::scene::ALLOW_ZWRITE_ON_TRANSPARENT, true);
	guienv = device->getGUIEnvironment();

	inited = true;

	return 0;
}

void App::createNode()
{
	if (!inited)
		throw std::runtime_error("App should be inited before use!");
	auto *mesh = scene->getMesh("data/model/stick/stickfigure2.obj");
	if (!mesh)
		throw std::runtime_error("Could not load mesh!");
	node = scene->addAnimatedMeshSceneNode(mesh);
	if (node)
	{
		node->setMaterialFlag(irr::video::EMF_LIGHTING, false);
		node->setMD2Animation(irr::scene::EMAT_STAND);
		node->setMaterialTexture(0, driver->getTexture("data/model/stick/stickfigure2.bmp"));
	}

	scene->addCameraSceneNode(nullptr, vector3df(0, 0, 5), vector3df(0, 0, 0));
}

void App::render() const
{
	auto rotate = 0.025f;
	auto vert = node->getRotation();
	node->setRotation(vector3df(vert.X + rotate, vert.Y + rotate, vert.Z + rotate));

	driver->beginScene(true, true, irr::video::SColor(250, 0, 0, 0));
	scene->drawAll();
	guienv->drawAll();
	driver->endScene();
}

void App::advance() const
{
	device->getTimer()->tick();
}


LRESULT WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		// Signal application to terminate
		PostQuitMessage(0);
		return 0;

	case WM_KEYDOWN:
		// If ESC has been pressed then signal window should close
		if (LOWORD(wParam) == VK_ESCAPE) SendMessage(hWnd, WM_CLOSE, NULL, NULL);
		break;

	case WM_LBUTTONDOWN:
		// Trick OS into thinking we are dragging on title bar for any clicks on the main window
		SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, NULL);
		return TRUE;

	case WM_ERASEBKGND:
		// We dont want to call render twice so just force Render() in WM_PAINT to be called
		SendMessage(hWnd, WM_PAINT, NULL, NULL);
		return TRUE;

	case WM_PAINT:
		// Force a render to keep the window updated
		App::instance().render();
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// +-----------+
// | WinMain() |
// +-----------+---------+
// | Program entry point |
// +---------------------+
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, INT)
{

	App &app = App::instance();

	HWND       hWnd = NULL;
	MSG        uMsg;
	WNDCLASSEX wc = { sizeof(WNDCLASSEX),              // cbSize
		NULL,                            // style
		WindowProc,                      // lpfnWndProc
		NULL,                            // cbClsExtra
		NULL,                            // cbWndExtra
		hInstance,                       // hInstance
		LoadIcon(NULL, IDI_APPLICATION), // hIcon
		LoadCursor(NULL, IDC_ARROW),     // hCursor
		NULL,                            // hbrBackground
		NULL,                            // lpszMenuName
		g_wcpAppName,                    // lpszClassName
		LoadIcon(NULL, IDI_APPLICATION) };// hIconSm

	RegisterClassEx(&wc);
	hWnd = CreateWindowEx(WS_EX_COMPOSITED,             // dwExStyle
		g_wcpAppName,                 // lpClassName
		g_wcpAppName,                 // lpWindowName
		WS_POPUP,        // dwStyle
		CW_USEDEFAULT, CW_USEDEFAULT, // x, y
		g_iWidth, g_iHeight,          // nWidth, nHeight
		NULL,                         // hWndParent
		NULL,                         // hMenu
		hInstance,                    // hInstance
		NULL);                        // lpParam

									  // Extend glass to cover whole window
	DwmExtendFrameIntoClientArea(hWnd, &g_mgDWMMargins);

	try
	{
		app.init(hWnd);
		app.createNode();

		// Show the window
		ShowWindow(hWnd, SW_SHOWDEFAULT);
		UpdateWindow(hWnd);

		// Enter main loop
		while (TRUE)
		{
			// Check for a message
			if (PeekMessage(&uMsg, NULL, 0, 0, PM_REMOVE))
			{
				// Check if the message is WM_QUIT
				if (uMsg.message == WM_QUIT)
				{
					// Break out of main loop
					break;
				}

				// Pump the message
				TranslateMessage(&uMsg);
				DispatchMessage(&uMsg);
			}

			app.advance();
			// Render a frame
			app.render();
		}


	} catch(const std::exception &e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
	}


	
	return 0;
}
