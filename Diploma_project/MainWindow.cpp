#include "MainWindow.h"

std::vector<LPCSTR> MainWindow::labelText{"Current File", "Votage File", "Qc", "dT", "Th"};
std::vector<std::string> MainWindow::options;
bool MainWindow::crt = true;
constexpr int OK_BUTTON = 3001;

LRESULT CALLBACK MainWindow::MessageRouter(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	MainWindow *mainWnd;
	HWND handler;

	if (msg == WM_CREATE)
	{
		mainWnd = (MainWindow*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)mainWnd);
	}
	else
		mainWnd = (MainWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	return mainWnd->thisWndProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK MainWindow::thisWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	int yPos = 10;
	LPSTR bufStr;

	switch (msg)
	{
	case WM_CREATE:
	{
		crt = false;
		for (; yPos <= 40; yPos += 30)
		{
			HWND handler;
			handler = CreateWindow("Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 10, yPos, 200, 20, hWnd, NULL,
				GetModuleHandle(NULL), NULL);
			CreateWindow("Static", MainWindow::labelText.at(yPos / 30), WS_CHILD | WS_VISIBLE, 230, yPos, 100, 20, hWnd, NULL, GetModuleHandle(NULL), NULL);
			this->EditHandlers.push_back(handler);
		}
		CreateWindow("Static", "Zero level", WS_CHILD | WS_VISIBLE, 10, yPos, 100, 20, hWnd, NULL, GetModuleHandle(NULL), NULL);
		CreateWindow("Static", "Variation", WS_CHILD | WS_VISIBLE, 120, yPos, 100, 20, hWnd, NULL, GetModuleHandle(NULL), NULL);
		for (yPos += 30; yPos <= 160; yPos += 30)
		{
			HWND handler;
			handler = CreateWindow("Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 10, yPos, 100, 20, hWnd, NULL,
				GetModuleHandle(NULL), NULL);
			CreateWindow("Static", MainWindow::labelText.at(yPos / 30 - 1), WS_CHILD | WS_VISIBLE, 230, yPos, 100, 20, hWnd, NULL, GetModuleHandle(NULL), NULL);
			this->EditHandlers.push_back(handler);
			handler = CreateWindow("Edit", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 120, yPos, 100, 20, hWnd, NULL,
				GetModuleHandle(NULL), NULL);
			this->EditHandlers.push_back(handler);
		}
		CreateWindow("Button", "OK", WS_VISIBLE | BS_DEFPUSHBUTTON | WS_CHILD, 220, yPos + 50, 50, 20, hWnd, (HMENU)OK_BUTTON, GetModuleHandle(NULL), NULL);
		for (auto hdl : {EditHandlers.at(0), EditHandlers.at(1)})
			SetWindowSubclass(hdl, childProc, 0, 0);

		if (!MainWindow::options.empty())
		{
			for (int i = 0; i < EditHandlers.size(); i++)
			{
				SetWindowText(EditHandlers.at(i), MainWindow::options.at(i).c_str());
			}
		}
		break;
	}
	case WM_COMMAND:
		switch (wParam)
		{
		case OK_BUTTON:
		{
			options.clear();
			crt = true;

			for (auto hdl : EditHandlers)
			{
				bufStr = new TCHAR[GetWindowTextLength(hdl) + 1];
				GetWindowText(hdl, bufStr, GetWindowTextLength(hdl) + 1);
				options.push_back(bufStr);
				delete[] bufStr;
			}

			/*if (checkFile(options.at(0)) && checkFile(options.at(1)))
			{
				for (int i : {0, 1})
					if (getFileFormat(options.at(i)) == "png")
					{
						DigitizerWindow digWnd(GetModuleHandle(NULL), "DigWnd", options.at(i));
						options.at(i) = digWnd.run();
					}
			}
			else
			{
				MessageBox(hWnd, "Invalid File Paths", "Error", NULL);
				crt = false;
			}*/

			if (std::count_if(options.begin(), options.end(), [](std::string& str) {return str.empty(); }) == 0)
			{
				if (checkFile(options.at(0)) && checkFile(options.at(1)))
				{
					for (int i : {0, 1})
						if (getFileFormat(options.at(i)) == "png")
						{
							DigitizerWindow digWnd(GetModuleHandle(NULL), "DigWnd", options.at(i));
							options.at(i) = digWnd.run();
						}
				}
				else
				{
					MessageBox(hWnd, "Invalid File Paths", "Error", NULL);
					crt = false;
				}
			}
			else
			{
				crt = false;
				MessageBox(hWnd, "Fill all fields", "Error", NULL);
			}
			if (std::count_if(options.begin(), options.end(), [](std::string& str) {return str.empty(); }) != 0) crt = false;

			if (crt) DestroyWindow(hWnd);
			else
			{
				MessageBox(hWnd, "Data is not collected", "Error", NULL);
				if (!MainWindow::options.empty())
				{
					for (int i = 0; i < EditHandlers.size(); i++)
						SetWindowText(EditHandlers.at(i), MainWindow::options.at(i).c_str());
				}
			}
		}
			break;
		default:
			break;
		}
	break;
	case WM_DESTROY:
	{
		if (!crt) MainWindow::options.clear();
		PostQuitMessage(0);
		break;
	}
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

int MainWindow::run()
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

MainWindow::MainWindow(HINSTANCE hInstance, LPCTSTR className)
{
	crt = false;
	WNDCLASS wcWindowClass = { 0 };
	wcWindowClass.lpfnWndProc = (WNDPROC)MessageRouter;
	wcWindowClass.style = CS_HREDRAW | CS_VREDRAW;
	wcWindowClass.hInstance = hInstance;
	wcWindowClass.lpszClassName = className;
	wcWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcWindowClass.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;
	RegisterClass(&wcWindowClass);

	hWnd = CreateWindow(className, "Options", WS_VISIBLE | WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, NULL, NULL, hInstance, this);
}

MainWindow::~MainWindow()
{
	DestroyWindow(hWnd);
}

LRESULT CALLBACK MainWindow::childProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR)
{
	switch (msg)
	{
	case WM_LBUTTONDBLCLK:
	{
		SetWindowText(hWnd, getFileName(hWnd));
		break;
	}
	default:
		return DefSubclassProc(hWnd, msg, wParam, lParam);
		break;
	}
}

LPSTR MainWindow::getFileName(HWND hWnd)
{
	TCHAR szFile[255] = { 0 };

	OPENFILENAME opf;
	ZeroMemory(&opf, sizeof(opf));
	opf.lStructSize = sizeof(opf);
	opf.hwndOwner = hWnd;
	opf.lpstrFile = szFile;
	opf.nMaxFile = sizeof(szFile);
	opf.lpstrFilter = ("csv\0*.csv\0Png\0*.png\0");
	opf.nFilterIndex = 2;
	opf.lpstrFileTitle = NULL;
	opf.nMaxFileTitle = 0;
	opf.lpstrInitialDir = NULL;
	opf.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	return GetOpenFileName(&opf) == TRUE ? opf.lpstrFile : nullptr;
}

std::string MainWindow::getFileFormat(std::string file)
{
	if (file.size() >= 4)
	{
		std::string format;
		auto rIt = file.rbegin();
		while (format.size() != 3)
		{
			format += *rIt;
			rIt++;
		}
		std::reverse(format.begin(), format.end());
		std::transform(format.begin(), format.end(), format.begin(), ::tolower);
		return format;
	}
	return nullptr;
}

bool MainWindow::checkFile(std::string filePath)
{
	std::regex reg("[A-Z]:(\\\\+(.)*)*\\\\+[a-zA-Z]+\\.[a-zA-Z]+");
	std::smatch match;
	std::regex_search(filePath, match, reg);

	return (match[0] == filePath) ? true : false;
}