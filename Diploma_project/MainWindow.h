#include <Windows.h>
#include "Libs.h"
#include <CommCtrl.h>
#include "DigitizerWindow.h"

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
	static std::vector<std::string> options;
	static std::vector<LPCSTR> labelText;
	static LPSTR getFileName(HWND);
	std::string getFileFormat(std::string);
	bool checkFile(std::string);
public:
	MainWindow(HINSTANCE, LPCTSTR);
	HWND getHandlerWindow() { return hWnd; }
	int run();
	std::vector<std::string>& getOptions() { return options; }
	~MainWindow();
};