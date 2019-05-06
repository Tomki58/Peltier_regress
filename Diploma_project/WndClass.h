#include <Windows.h>

#pragma once
class WndClass
{
protected:
	static LRESULT CALLBACK MessageRouter(HWND, UINT, WPARAM, LPARAM);
	virtual LRESULT CALLBACK thisWndProc(HWND, UINT, WPARAM, LPARAM) = 0;
	WNDCLASS wcWindowClass;
	HWND hWnd;
public:
	WndClass(HINSTANCE hInstance, LPCTSTR className);
	virtual int run();
	~WndClass();
};

