#pragma once
#include "WndClass.h"
#include "OptionsWnd.h"
#include "Matrix.h"
#include "FunctionHandler.h"

#pragma comment(lib, "ComCtl32.Lib")

class MainWndClass :
	public WndClass
{
private:
	HWND hGraphButton, hBrowseButton;
	LRESULT CALLBACK thisWndProc(HWND, UINT, WPARAM, LPARAM) override;
	std::unique_ptr<TApplication> tAPP;
	int SetListViewColumns(HWND hWndLV, int colNum, int textMaxLen, char** header);
	BOOL WINAPI AddListViewItems(HWND hWndLV, int colNum, int textMaxLen, const std::vector<std::pair<int, double>> num_value);
public:
	MainWndClass(HINSTANCE hInstance, LPCTSTR className);
	int run();
	~MainWndClass();
};

