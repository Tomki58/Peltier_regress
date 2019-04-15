#include "DigitizerWindow.h"

constexpr unsigned int H_OKBUTTON = 3002;
bool DigitizerWindow::clicked;
bool DigitizerWindow::lcFlag;
int DigitizerWindow::counter = 0;
std::string DigitizerWindow::path;
std::vector<std::pair<POINT, DigitizerWindow::Type>> DigitizerWindow::pntVector;
std::vector<POINT> DigitizerWindow::xPoints, DigitizerWindow::values;
std::vector<std::pair<POINT, double>> DigitizerWindow::yPoints;
RECT DigitizerWindow::wndRect;
DigitizerWindow::Type DigitizerWindow::type;

LRESULT CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

// TODO:
// Доработать лоигку оцифровщика
// Пофиксить отрисовку линий

void DigitizerWindow::getNextType(Type& type)
{
	switch (type)
	{
	case Type::X_axis:
		type = Type::Y_axis;
		break;
	case Type::Y_axis:
		type = Type::Values;
		break;
	default:
		type = Type::X_axis;
		break;
	}
}

ATOM DigitizerWindow::RegMyWindowClass(HINSTANCE hInstance, LPCTSTR lpzClassName, HBRUSH color)
{
	WNDCLASS wcWindowClass = { 0 };
	// адрес ф-ции обработки сообщений
	wcWindowClass.lpfnWndProc = (WNDPROC)MessageRouter;
	// стиль окна
	wcWindowClass.style = CS_HREDRAW | CS_VREDRAW;
	// дискриптор экземпл¤ра приложени¤
	wcWindowClass.hInstance = hInstance;
	// название класса
	wcWindowClass.lpszClassName = lpzClassName;
	// загрузка курсора
	wcWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	// загрузка цвета окон
	wcWindowClass.hbrBackground = color;//(HBRUSH)COLOR_WINDOWFRAME;

	return RegisterClass(&wcWindowClass); // регистраци¤ класса
}

LRESULT DigitizerWindow::MessageRouter(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	DigitizerWindow* digitWnd;
	HWND handler;



	if (msg == WM_CREATE)
	{
		/*MessageBox(hWnd, "Error", NULL, NULL);*/
		digitWnd = (DigitizerWindow*)(((LPCREATESTRUCT)lParam)->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)digitWnd);
	}
	else
		digitWnd = (DigitizerWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	return digitWnd->thisWndProc(hWnd, msg, wParam, lParam);
}

LRESULT DigitizerWindow::thisWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	{
		RECT screen_rect;
		GetWindowRect(hWnd, &screen_rect);
		// локальные переменные
		// выборка и обработка сообщений
		switch (msg)
		{
		case WM_CREATE:
		{
			HWND hOkButton = CreateWindow("button", "Calculate", WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 600, 400, 90, 20, hWnd, (HMENU)H_OKBUTTON, (HINSTANCE)GetModuleHandle(NULL), NULL);

			LONG hInstance = GetWindowLong(hWnd, GWL_HINSTANCE);
			LPCTSTR	cnvsWndClass = TEXT("Canvas subclass");
			LPCTSTR cnvsWndClass_1 = TEXT("Canvas subclass_1");
			HWND hCnvsWnd = NULL;

			switch (counter)
			{
			case 0:
			{
				if (!RegMyWindowClass((HINSTANCE)hInstance, cnvsWndClass, (HBRUSH)COLOR_WINDOWFRAME))
					return 1;
				hCnvsWnd = CreateWindow(cnvsWndClass, TEXT("Canvas"),
					WS_VISIBLE | WS_BORDER | WS_CHILD, 0, 0, screen_rect.right - screen_rect.left, 400, hWnd, NULL,
					(HINSTANCE)hInstance, this);
			}
			break;
			default:
			{
				if (!RegMyWindowClass((HINSTANCE)hInstance, cnvsWndClass_1, (HBRUSH)COLOR_WINDOWFRAME))
					return 1;
				hCnvsWnd = CreateWindow(cnvsWndClass_1, TEXT("Canvas"),
					WS_VISIBLE | WS_BORDER | WS_CHILD, 0, 0, screen_rect.right - screen_rect.left, 400, hWnd, NULL,
					(HINSTANCE)hInstance, this);
			}
			break;
			}

			GetWindowRect(hCnvsWnd, &wndRect);
			SetWindowSubclass(hCnvsWnd, childProc, 0, 0);
		}
		break;
		case WM_MOVE:
		{
			GetWindowRect(hWnd, &screen_rect);
		}
		break;
		case WM_COMMAND:
			switch (wParam)
			{
			case H_OKBUTTON:
			{
				newFilePath = path + ".csv";
				std::ofstream fileStream;
				fileStream.open(newFilePath);
				double valPerX = abs(xPoints.at(0).x - xPoints.at(1).x) / 20.0;
				double valPerY = abs(yPoints.at(0).first.y - yPoints.at(1).first.y) / abs(yPoints.at(0).second - yPoints.at(1).second);
				
				int idx = 0;

				for (auto& val : values)
				{
					/*int idx = round((val.x - xPoints.at(0).x) / valPerX);*/
					double yValue = abs(val.y - yPoints.at(0).first.y) / valPerY;
					std::string output = std::to_string(idx) + ";" + std::to_string(yValue) + "\n";
					fileStream << output;
					idx++;
				}
			}
			counter++;
			DestroyWindow(hWnd);
			break;
			default:
				break;
			}
			break;
		case WM_DESTROY:
		{
			counter++;
			PostQuitMessage(0);
		}
		break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
		return 0;
	}
}

LRESULT DigitizerWindow::childProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR)
{
	POINT pt;
	RECT pntRect;
	switch (msg)
	{
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case VK_CONTROL:
			getNextType(type);
			// добавление изменения типа курсора
			break;
		case VK_TAB:
			MessageBeep(MB_OK);
		default:
			break;
		}
	}
	case WM_PAINT:
	{
		PAINTSTRUCT pt;
		HDC hDC = BeginPaint(hWnd, &pt);
		HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
		HPEN hPen_x = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
		HPEN hPen_y = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
		HPEN hPen_v = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));

		HGDIOBJ hOldPen = SelectObject(hDC, hPen);
		GetWindowRect(hWnd, &pntRect);

		std::wstring wstr(path.begin(), path.end());

		if (lcFlag)
		{

			Gdiplus::Graphics graphics(hDC);
			Gdiplus::Image gdiImage(wstr.c_str());

			graphics.DrawImage(&gdiImage, 0, 0, wndRect.right - wndRect.left, wndRect.bottom - wndRect.top);

			lcFlag = false;
		}

		if (xPoints.size() == 2)
		{
			hPen = hPen_x;
			hOldPen = SelectObject(hDC, hPen);
			MoveToEx(hDC, xPoints.at(0).x, xPoints.at(0).y, nullptr);
			LineTo(hDC, xPoints.at(1).x, xPoints.at(1).y);
		}

		if (yPoints.size() == 2)
		{
			hPen = hPen_y;
			hOldPen = SelectObject(hDC, hPen);
			MoveToEx(hDC, yPoints.at(0).first.x, yPoints.at(0).first.y, nullptr);
			LineTo(hDC, yPoints.at(1).first.x, yPoints.at(1).first.y);
		}

		if (values.size() > 1)
		{
			std::sort(values.begin(), values.end(), [](POINT pt1, POINT pt2) { return pt1.x < pt2.x; });
			hPen = hPen_v;
			hOldPen = SelectObject(hDC, hPen);

			auto it_1 = values.begin();
			auto it_2 = it_1 + 1;
			while (it_2 != values.end())
			{
				MoveToEx(hDC, (*it_1).x, (*it_1).y, nullptr);
				LineTo(hDC, (*it_2).x, (*it_2).y);
				it_1 = it_2++;
			}
		}

		SelectObject(hDC, hOldPen);
		DeleteObject(hPen);

		EndPaint(hWnd, &pt);
	}
	break;
	case WM_CREATE:
	{
		InvalidateRect(hWnd, &wndRect, false);
		UpdateWindow(hWnd);
	}
	break;
	case WM_LBUTTONDOWN:
	{
		SetFocus(hWnd);
		GetCursorPos(&pt);
		ScreenToClient(hWnd, &pt);
		switch (type)
		{
		case Type::X_axis:
		{
			if (xPoints.size() < 2)
				xPoints.push_back(pt);
		}
		break;
		case Type::Y_axis:
		{
			if (yPoints.size() < 2)
			{
				yPoints.push_back({ pt, 0 });
				INT_PTR hDLG = DialogBox((HINSTANCE)GetWindowLong(hWnd, GWLP_HINSTANCE), MAKEINTRESOURCE(IDD_DLGFIRST), hWnd, reinterpret_cast<DLGPROC>(DlgProc));
				ShowWindow((HWND)hDLG, SW_SHOW);
			}
		}
		break;
		default:
		{
			values.push_back(pt);
		}
		break;
		}

		InvalidateRect(hWnd, &wndRect, false);
		UpdateWindow(hWnd);

	break;
	}
	default:
		return DefSubclassProc(hWnd, msg, wParam, lParam);
	}
}

DigitizerWindow::DigitizerWindow(HINSTANCE hInstance, LPCTSTR className, std::string fileName)
{
	clicked = false;
	lcFlag = true;
	type = Type::X_axis;
	path = fileName;

	WNDCLASS wcWindowClass = { 0 };
	wcWindowClass.lpfnWndProc = (WNDPROC)MessageRouter;
	wcWindowClass.style = CS_HREDRAW | CS_VREDRAW;
	wcWindowClass.hInstance = hInstance;
	wcWindowClass.lpszClassName = className;
	wcWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcWindowClass.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;
	RegisterClass(&wcWindowClass);

	RECT screen_rect; 
	GetWindowRect(GetDesktopWindow(), &screen_rect);

	HWND hWnd = CreateWindow(className, TEXT("Digitizer"),
		WS_VISIBLE | WS_SYSMENU | WS_OVERLAPPED | CS_HREDRAW, 0, 0, screen_rect.right / 2, screen_rect.bottom / 2, NULL, NULL,
		hInstance, this);
}


DigitizerWindow::~DigitizerWindow()
{
	pntVector.clear();
	xPoints.clear();
	yPoints.clear();
	values.clear();
	type = Type::X_axis;
	DestroyWindow(hWnd);
}

std::string DigitizerWindow::run()
{
	clicked = false;
	lcFlag = true;
	xPoints.clear(); yPoints.clear(); values.clear();
	ULONG_PTR m_gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

	MSG msg{ 0 };
	if (hWnd != INVALID_HANDLE_VALUE)
	{
		while (GetMessage(&msg, 0, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	Gdiplus::GdiplusShutdown(m_gdiplusToken);

	return newFilePath;
}

LRESULT CALLBACK DigitizerWindow::DlgProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	LPSTR bufStr;
	switch (Msg)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_COMMAND:
		switch (wParam)
		{
		case IDOK:
		{
			HWND hEditBox = FindWindowEx(hWndDlg, NULL, "Edit", NULL);
			bufStr = new TCHAR[GetWindowTextLength(hEditBox) + 1];
			if (hEditBox && (GetWindowTextLength(hEditBox) != 0))
			{
				GetWindowText(hEditBox, bufStr, GetWindowTextLength(hEditBox) + 1);
				auto it = yPoints.end(); it--;
				(*it).second = atof(bufStr);
			}
			EndDialog(hWndDlg, 0);
		}
		}
		break;
	}

	return FALSE;
}