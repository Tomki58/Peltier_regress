#include <Windows.h>
#include <vector>
#include <CommCtrl.h>
#include <algorithm>

#pragma comment(lib, "Comctl32.lib")

#pragma once
class MainWindow
{
private:
	DWORD window_style;
	static bool crt;
	static LRESULT CALLBACK MessageRouter(HWND, UINT, WPARAM, LPARAM);
	LRESULT CALLBACK thisWndProc(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK childProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
	HWND hWnd;
	std::vector<HWND> EditHandlers;
	std::vector<std::string> options;
	static std::vector<LPCSTR> labelText;
	static LPSTR getFileName(HWND);
public:
	MainWindow(HINSTANCE, LPCTSTR);
	HWND getHandlerWindow() { return hWnd; }
	int run();
	std::vector<std::string>& getOptions() { return options; }
	~MainWindow();
};

