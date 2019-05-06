#include <Windows.h>
#include "WndClass.h"
#include "Libs.h"
#include <CommCtrl.h>
#include "DigitizerWindow.h"

#pragma comment(lib, "Comctl32.lib")

#pragma once
class OptionsWnd : public WndClass
{
private:
	DWORD window_style;
	static bool crt;
	LRESULT CALLBACK thisWndProc(HWND, UINT, WPARAM, LPARAM) override;
	static LRESULT CALLBACK childProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
	std::vector<HWND> EditHandlers;
	static std::vector<std::string> options;
	static std::vector<LPCSTR> labelText;
	static LPSTR getFileName(HWND);
	std::string getFileFormat(std::string);
	bool checkFile(std::string);
public:
	OptionsWnd(HINSTANCE, LPCTSTR);
	HWND getHandlerWindow() { return hWnd; }
	std::vector<std::string>& getOptions() { return options; }
	~OptionsWnd();
};