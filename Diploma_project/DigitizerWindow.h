#include "Libs.h"
#include <Windows.h>

#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "gdiplus.lib")

#pragma once
class DigitizerWindow
{
private:
	static enum class Type { X_axis, Y_axis, Values };
	static void getNextType(Type&);
	static Type type;
	static std::string path;
	static int counter;
	std::string newFilePath;
	HWND hWnd;
	static bool clicked, lcFlag;
	static RECT wndRect;
	static std::vector<std::pair<POINT, Type>> pntVector;
	static std::vector<POINT> xPoints, values;
	static std::vector<std::pair<POINT, double>> yPoints;
	ATOM RegMyWindowClass(HINSTANCE, LPCTSTR, HBRUSH);
	static LRESULT CALLBACK MessageRouter(HWND, UINT, WPARAM, LPARAM);
	LRESULT CALLBACK thisWndProc(HWND, UINT, WPARAM, LPARAM);
	static LRESULT CALLBACK childProc(HWND, UINT, WPARAM, LPARAM, UINT_PTR, DWORD_PTR);
	static LRESULT CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
public:
	DigitizerWindow(HINSTANCE, LPCTSTR, std::string);
	~DigitizerWindow();
	std::string run();
};