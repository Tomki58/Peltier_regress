#include "Matrix.h"
#include "FunctionHandler.h"
#include "MainWindow.h"


#pragma comment(lib, "ComCtl32.Lib")

constexpr int GRAPH_BUTTON_ID = 3001;
constexpr int OPTIONS_BUTTON_ID = 3002;
constexpr int IDC_LISTVIEW_ID_1 = 3003;
constexpr int IDC_LISTVIEW_ID_2 = 3004;
constexpr LPCTSTR optWndClass = "OptWnd";

static std::string FILE_PATH;
static std::vector<std::string> options;

static const char* header[] = { "Num", "Value" };

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM RegMyWindowClass(HINSTANCE, LPCTSTR);
int SetListViewColumns(HWND hWndLV, int colNum, int textMaxLen, char** header);
BOOL WINAPI AddListViewItems(HWND hWndLV, int colNum, int textMaxLen, const std::vector<std::pair<int, double>> num_value);


std::unique_ptr<TApplication> tAPP(new TApplication("App", 0, 0));

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE         hPrevInstance,
	LPSTR             lpCmdLine,
	int               nCmdShow)

{
	LPCTSTR lpzClass = TEXT("My Window Class!");
	LPCTSTR buttonClass = TEXT("Button Class");

	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icex);

	Matrix_Plan::fill_table("C:\\AppData\\Fisher_table.csv");


	if (!RegMyWindowClass(hInstance, lpzClass))
		return 1;

	RECT screen_rect;
	GetWindowRect(GetDesktopWindow(), &screen_rect); // ���������� ������
	int x = screen_rect.right / 2 - 150;
	int y = screen_rect.bottom / 2 - 75;

	// �������� ����������� ����
	HWND hWnd = CreateWindow(lpzClass, TEXT("Chart"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 500, 400, NULL, NULL,
		hInstance, NULL);

	HWND hGraphButton = CreateWindow("BUTTON", "Graph",
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_DISABLED, 400, 300, 80, 20,
		hWnd, (HMENU)GRAPH_BUTTON_ID, hInstance, NULL);

	HWND hBrowseButton = CreateWindow("BUTTON", "Options",
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 400, 330, 80, 20,
		hWnd, (HMENU)OPTIONS_BUTTON_ID, hInstance, NULL);

	

	// ���� ��������� ����������
	MSG msg = { 0 };    // ��������� ���������
	int iGetOk = 0;   // ���������� ���������
	while ((iGetOk = GetMessage(&msg, hWnd, 0, 0)) != 0) // ���� ���������
	{
		if (iGetOk == -1) return 3;  // ���� GetMessage ������ ������ - �����
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return 0; 
}


ATOM RegMyWindowClass(HINSTANCE hInst, LPCTSTR lpzClassName)
{
	WNDCLASS wcWindowClass = { 0 };
	// ����� �-��� ��������� ���������
	wcWindowClass.lpfnWndProc = (WNDPROC)WndProc;
	// ����� ����
	wcWindowClass.style = CS_HREDRAW | CS_VREDRAW;
	// ���������� ���������� ����������
	wcWindowClass.hInstance = hInst;
	// �������� ������
	wcWindowClass.lpszClassName = lpzClassName;
	// �������� �������
	wcWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	// �������� ����� ����
	wcWindowClass.hbrBackground = (HBRUSH)COLOR_APPWORKSPACE;
	return RegisterClass(&wcWindowClass); // ����������� ������
}

////////////////////////////////////////////////////////////////////////// 
// ������� ��������� ���������
LRESULT CALLBACK WndProc(
	HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	// ������� � ��������� ���������
	switch (message)
	{
	case WM_COMMAND:
		switch (wParam)
		{
		// ��������� ������� �� ������ Graph
		case GRAPH_BUTTON_ID:
		{
			FileHandler fh_current(options.at(0));
			Matrix_Plan mtrx_current{ 3 };
			mtrx_current.read_from_file(fh_current);
			mtrx_current.count();
			mtrx_current.assessment();
			Matrix_Plan mtrx_voltage{ 3 };
			FileHandler fh_voltage(options.at(1));
			mtrx_voltage.read_from_file(fh_voltage);
			mtrx_voltage.count();
			mtrx_voltage.assessment();

			FunctionHandler *hFnc = new FunctionHandler{ {mtrx_current, mtrx_voltage}, options };
			hFnc->initialize_graphs(tAPP);
		}
		break;
		// ��������� ������� �� ������ Browse
		case OPTIONS_BUTTON_ID:
		{
			MainWindow mainWnd((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), const_cast<LPCTSTR>(optWndClass));
			mainWnd.run();
			options = mainWnd.getOptions();

			HWND childWnd = FindWindowEx(hWnd, NULL, "BUTTON", NULL);
			EnableWindow(childWnd, TRUE);
			if (!options.empty())
			{
				while (HWND hDelWnd = FindWindowEx(hWnd, NULL, WC_LISTVIEW, NULL))
					DestroyWindow(hDelWnd);

				

				HWND hWndLV_1 = CreateWindow(WC_LISTVIEW, "", WS_CHILD | LVS_REPORT | WS_VISIBLE,
					10, 10, 200, 200, hWnd, (HMENU)IDC_LISTVIEW_ID_1, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
				HWND hWndLV_2 = CreateWindow(WC_LISTVIEW, "", WS_CHILD | LVS_REPORT | WS_VISIBLE,
					250, 10, 200, 200, hWnd, (HMENU)IDC_LISTVIEW_ID_2, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
				ListView_SetExtendedListViewStyleEx(hWndLV_1, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
				ListView_SetExtendedListViewStyleEx(hWndLV_2, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
				SetListViewColumns(hWndLV_1, 2, 6, const_cast<char**>(header));
				SetListViewColumns(hWndLV_2, 2, 6, const_cast<char**>(header));
				FileHandler fh_listView1{ options.at(0) };
				FileHandler fh_listView2{ options.at(1) };
				AddListViewItems(hWndLV_1, 2, 6, fh_listView1.get_pair());
				AddListViewItems(hWndLV_2, 2, 6, fh_listView2.get_pair());
			}
			else EnableWindow(childWnd, FALSE);
		}
		default:
			break;
		}
		break;
	case WM_DESTROY:
	{
		tAPP->Terminate();
		PostQuitMessage(0);  // ������� �� ���������
	}
		break;
	default:
		// ��� ��������� �� ������������ ���� ���������� ���� Windows
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int SetListViewColumns(HWND hWndLV, int colNum, int textMaxLen, char** header)
{
	RECT rcl;
	GetClientRect(hWndLV, &rcl);

	int index = -1;

	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT | LVCF_WIDTH;
	lvc.cx = (rcl.right - rcl.left) / colNum;
	lvc.cchTextMax = textMaxLen;

	for (int i = 0; i < colNum; i++)
	{
		lvc.pszText = header[i];
		index = ListView_InsertColumn(hWndLV, i, &lvc);
		if (index == -1) break;
	}

	return index;
}

BOOL WINAPI AddListViewItems(HWND hWndLV, int colNum, int textMaxLen, const std::vector<std::pair<int, double>> num_value)
{
	LVITEM lvI;

	// Initialize LVITEM members that are common to all items.
	lvI.pszText = LPSTR_TEXTCALLBACK; // Sends an LVN_GETDISPINFO message.
	lvI.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvI.stateMask = 0;
	lvI.iSubItem = 0;
	lvI.state = 0;

	// Initialize LVITEM members that are different for each item.
	for (int index = 0; index < num_value.size(); index++)
	{
		lvI.iItem = index;
		lvI.iImage = index;

		char first_val[3];
		char second_val[10];

		_itoa_s(num_value[index].first, first_val, 10);
		sprintf_s(second_val, "%f", num_value[index].second);


		// Insert items into the list.
		if (ListView_InsertItem(hWndLV, &lvI) == -1)
			return FALSE;
		ListView_SetItemText(hWndLV, index, 0, first_val);
		ListView_SetItemText(hWndLV, index, 1, second_val);
	}

	return TRUE;
}