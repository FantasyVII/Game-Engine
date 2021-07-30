#include <iostream>
#include <Windows.h>
#include <GL/glew.h>
#include <GL/wglew.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

const wchar_t CLASS_NAME[] = L"OpenGL Program";

const int windowPositionX = 100;
const int windowPositionY = 200;

const int windowWidth = 1280;
const int windowHeight = 720;

HINSTANCE hInstance;
HWND hwnd;
HGLRC hglrc;
HDC hdc;

HWND CreateWinAPIWindow()
{
	HWND hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		CLASS_NAME,
		WS_OVERLAPPEDWINDOW,
		windowPositionX,
		windowPositionY,
		windowWidth,
		windowHeight,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (hwnd == NULL)
	{
		std::cout << "Window failed to be created!" << std::endl;
		return 0;
	}

	return hwnd;
}

int main()
{
	hInstance = GetModuleHandle(NULL);

	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	hwnd = CreateWinAPIWindow();

	//------------------------------------------- OPENGL ---------------------------

	const int bitColor = 32;
	const int depthBufferBit = 32;
	const int stencilBufferBit = 8;

	PIXELFORMATDESCRIPTOR pfd;
	ZeroMemory(&pfd, sizeof(pfd));      // set the pixel format for the DC

	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_TYPE_RGBA;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = bitColor;
	pfd.cDepthBits = depthBufferBit;
	pfd.cStencilBits = stencilBufferBit;
	pfd.iLayerType = PFD_MAIN_PLANE;

	hdc = GetDC(hwnd);
	int pixelFormat = ChoosePixelFormat(hdc, &pfd);

	if (pixelFormat > 0)
	{
		SetPixelFormat(hdc, pixelFormat, &pfd);
		HGLRC legacy_hglrc = wglCreateContext(hdc);

		if (!wglMakeCurrent(hdc, legacy_hglrc))
		{
			std::cout << "Failed to set the opengl current context\n";
		}

		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		}

		const int attribList[] =
		{
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_ACCELERATION_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB, bitColor,
			WGL_DEPTH_BITS_ARB, depthBufferBit,
			WGL_STENCIL_BITS_ARB, stencilBufferBit,
			0, // End
		};

		int pixelFormat;
		UINT numFormats;

		wglChoosePixelFormatARB(hdc, attribList, NULL, 1, &pixelFormat, &numFormats);

		if (pixelFormat < 0)
			std::cout << "Failed to create an opengl context with the following pixel format.\n";
		else
			std::cout << "Create an opengl context with the following pixel format.\n";

		std::cout << "\n" <<
			"WGL_DRAW_TO_WINDOW_ARB, GL_TRUE" << "\n" <<
			"WGL_ACCELERATION_ARB, GL_TRUE" << "\n" <<
			"WGL_SUPPORT_OPENGL_ARB, GL_TRUE" << "\n" <<
			"WGL_DOUBLE_BUFFER_ARB, GL_TRUE" << "\n" <<
			"WGL_SAMPLE_BUFFERS_ARB, GL_TRUE" << "\n" <<
			"WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB" << "\n" <<
			"WGL_COLOR_BITS_ARB, " << bitColor << "\n" <<
			"WGL_DEPTH_BITS_ARB, " << depthBufferBit << "\n" <<
			"WGL_STENCIL_BITS_ARB, " << stencilBufferBit << "\n";

		if (pixelFormat < 0)
			return 0;


		//------------------------ TEMP context ------------------------------------


		ReleaseDC(hwnd, hdc);
		hwnd = CreateWinAPIWindow();
		hdc = GetDC(hwnd);

		if (SetPixelFormat(hdc, pixelFormat, &pfd))
		{
			std::cout << "wglChoosePixelFormatARB was set correctly.\n";
		}
		else
			std::cout << "Could not set wglChoosePixelFormatARB.\n";

		int contextAttribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			WGL_CONTEXT_MINOR_VERSION_ARB, 6,
			WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0
		};

		hglrc = wglCreateContextAttribsARB(hdc, hglrc, contextAttribs);

		if (hglrc)
		{
			wglDeleteContext(legacy_hglrc);
			wglMakeCurrent(hdc, hglrc);
		}

		std::cout << "OPENGL VERSION: " << glGetString(GL_VERSION) << "\n";
	}
	//------------------------------------------- OPENGL ---------------------------

	ShowWindow(hwnd, SW_SHOW);
	MSG msg = { };

	//wglSwapIntervalEXT(60);

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {


			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(1.0, 0.0, 0.0, 1.0);
		SwapBuffers(hdc);
	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}