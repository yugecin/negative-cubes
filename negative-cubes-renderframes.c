
#define messages // so it doesn't freeze when clicking (unnecessary but ok)
#define nopopup // then screenshot works :^) (when also using "registerclass" and "messages")
#define XRES 640 // as per compo rules
#define YRES 360 // as per compo rules

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include "windows.h"
#include <GL/gl.h>
#include <GL/glext.h>
char *vsh=
"#version 430\n"
"layout (location=0) in vec2 i;"
"out vec2 p;"
"out gl_PerVertex"
"{"
"vec4 gl_Position;"
"};"
"void main() {"
"gl_Position=vec4(p=i,0.,1.);"
"}"
;
#include "frag.glsl.c"

PIXELFORMATDESCRIPTOR pfd={
	0, 1, PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER, 32, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0
};

WNDCLASSEX wc = {0};

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default: return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}
//gcc+ld? int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
//gcc+link? int WINAPI _WinMainCRTStartup(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
//gcc+crinkler void mainCRTStartup(void)
//gcc+crinkler subsystem:windows
void WinMainCRTStartup(void)
{
	char log[1024];
	int logsize;
	MSG msg;
	RECT rect;

	float uniformValues[4*2];
	int launchTimeMs, runningTimeMs, i, lastRenderedFrameRunningTimeMs = 0, glHandleTmp;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = DefWindowProc;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = (HINSTANCE)0x400000;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); /*large icon (alt tab)*/
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) COLOR_WINDOW;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "dominoesclass";
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION); /*small icon (taskbar)*/

	if (!RegisterClassEx(&wc)) {
		ExitProcess(0);
	}

	HANDLE hWnd = CreateWindowEx(WS_EX_APPWINDOW, wc.lpszClassName, "title", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, XRES, YRES, 0, 0, wc.hInstance, 0);
	HDC hDC = GetDC(hWnd);
	SetPixelFormat(hDC, ChoosePixelFormat(hDC, &pfd) , &pfd);
	wglMakeCurrent(hDC, wglCreateContext(hDC));

	// do shit to try to make sure that the window is showing before we hang while compiling shaders
	for (i = 0; i < 10; i++) {
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				goto done;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		SwapBuffers(hDC);
		Sleep(0);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	SwapBuffers(hDC);
	glClear(GL_COLOR_BUFFER_BIT);

	// do shit to try to make sure that the window is showing before we hang while compiling shaders
	for (i = 0; i < 10; i++) {
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				goto done;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		SwapBuffers(hDC);
		Sleep(0);
	}

	GLuint p = ((PFNGLCREATESHADERPROGRAMVPROC)wglGetProcAddress("glCreateShaderProgramv"))(GL_VERTEX_SHADER, 1, &vsh);
	GLuint s = ((PFNGLCREATESHADERPROGRAMVPROC)wglGetProcAddress("glCreateShaderProgramv"))(GL_FRAGMENT_SHADER, 1, &fragSource);
	((PFNGLGENPROGRAMPIPELINESPROC)wglGetProcAddress("glGenProgramPipelines"))(1, &glHandleTmp);
	((PFNGLBINDPROGRAMPIPELINEPROC)wglGetProcAddress("glBindProgramPipeline"))(glHandleTmp);
	((PFNGLUSEPROGRAMSTAGESPROC)wglGetProcAddress("glUseProgramStages"))(glHandleTmp, GL_VERTEX_SHADER_BIT, p);
	((PFNGLUSEPROGRAMSTAGESPROC)wglGetProcAddress("glUseProgramStages"))(glHandleTmp, GL_FRAGMENT_SHADER_BIT, s);
	logsize = 0;
	((PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog"))(s, sizeof(log), &logsize, log);
	if (log[0] && logsize) {
		MessageBoxA(NULL, log, "hi", MB_OK);
		ExitProcess(1);
	}
	((PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture"))(GL_TEXTURE0); // TODO: seems to work without, remove?
	glGenTextures(1, &glHandleTmp);
	glBindTexture(GL_TEXTURE_2D, glHandleTmp);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	launchTimeMs = GetTickCount();
	do
	{
		runningTimeMs = GetTickCount() - launchTimeMs;

		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				goto done;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (runningTimeMs - lastRenderedFrameRunningTimeMs > 50) {
			// should technically also bind the texture ... but it works without ..
			GetClientRect(hWnd, &rect);
			uniformValues[0] = rect.right;
			uniformValues[1] = rect.bottom;
			uniformValues[3] = runningTimeMs;
			((PFNGLPROGRAMUNIFORM4FVPROC)wglGetProcAddress("glProgramUniform4fv"))(s, 0, 2, uniformValues);
			glRecti(1,1,-1,-1);
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, rect.right, rect.bottom, 0);
			SwapBuffers(hDC);
			lastRenderedFrameRunningTimeMs = runningTimeMs;
		}
	} while (!GetAsyncKeyState(VK_ESCAPE));
done:
	ExitProcess(0);
}
