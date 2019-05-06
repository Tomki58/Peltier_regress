#include "WndClass.h"

LRESULT CALLBACK WndClass::MessageRouter(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	WndClass *mainWnd = nullptr;

	if (msg == WM_CREATE)
	{
		mainWnd = (WndClass*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)mainWnd);
	}
	else
		mainWnd = (WndClass*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	if (mainWnd)
		return mainWnd->thisWndProc(hWnd, msg, wParam, lParam);
	else
		return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WndClass::run()
{
	MSG msg{ 0 };
	if (hWnd != INVALID_HANDLE_VALUE)
	{
		while (GetMessage(&msg, 0, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}

WndClass::WndClass(HINSTANCE hInstance, LPCTSTR className)
{
	WNDCLASS wcWindowClass = { 0 };
	wcWindowClass.lpfnWndProc = (WNDPROC)MessageRouter;
	wcWindowClass.style = CS_HREDRAW | CS_VREDRAW;
	wcWindowClass.hInstance = hInstance;
	wcWindowClass.lpszClassName = className;
	wcWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcWindowClass.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;
	RegisterClass(&wcWindowClass);
}


WndClass::~WndClass()
{
}
