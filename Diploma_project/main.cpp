#include "MainWndClass.h"

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE         hPrevInstance,
	LPSTR             lpCmdLine,
	int               nCmdShow)

{
	MainWndClass mainWnd{ hInstance, "MainWndClass" };
	mainWnd.run();
}