#include "Matrix.h"
#include "FunctionHandler.h"
#include "MainWindow.h"


#pragma comment(lib, "ComCtl32.Lib")

constexpr int GRAPH_BUTTON_ID = 3001;
constexpr int BROWSE_BUTTON_ID = 3002;
constexpr int IDC_LISTVIEW_ID = 3003;
constexpr LPCTSTR optWndClass = "OptWnd";

static std::string FILE_PATH;
static std::vector<std::string> options;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM RegMyWindowClass(HINSTANCE, LPCTSTR);
int SetListViewColumns(HWND hWndLV, int colNum, int textMaxLen, char** header);
BOOL WINAPI AddListViewItems(HWND hWndLV, int colNum, int textMaxLen, const std::vector<std::pair<int, double>> num_value);
std::string getFileName(HWND hWnd);


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
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, screen_rect.right, screen_rect.bottom, NULL, NULL,
		hInstance, NULL);

	HWND hGraphButton = CreateWindow("BUTTON", "Graph",
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 10, 20, 90, 20,
		hWnd, (HMENU)GRAPH_BUTTON_ID, hInstance, NULL);

	HWND hBrowseButton = CreateWindow("BUTTON", "Browse...",
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 110, 20, 90, 20,
		hWnd, (HMENU)BROWSE_BUTTON_ID, hInstance, NULL);

	

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
			FileHandler fh_current(FILE_PATH);
			Matrix_Plan mtrx_current{ 3 };
			mtrx_current.read_from_file(fh_current);
			mtrx_current.count();
			mtrx_current.assessment();
			Matrix_Plan mtrx_voltage{ 3 };
			FileHandler fh_voltage("C:\\AppData\\Voltage.csv");
			mtrx_voltage.read_from_file(fh_voltage);
			mtrx_voltage.count();
			mtrx_voltage.assessment();

			FunctionHandler *hFnc = new FunctionHandler{ {mtrx_current, mtrx_voltage} };
			hFnc->initialize_graphs(tAPP);
		}
		break;
		// ��������� ������� �� ������ Browse
		case BROWSE_BUTTON_ID:
		{
			/*FILE_PATH = getFileName(hWnd);

			if (!FILE_PATH.empty())
			{
				HWND hWndLV = CreateWindow(WC_LISTVIEW, "", WS_CHILD | LVS_REPORT | WS_VISIBLE,
					500, 200, 200, 200, hWnd, (HMENU)IDC_LISTVIEW_ID, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);

				ListView_SetExtendedListViewStyleEx(hWndLV, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

				char *header[2];
				header[0] = const_cast<char*>("Num");
				header[1] = const_cast<char*>("Value");


				SetListViewColumns(hWndLV, 2, 6, header);
				FileHandler fh_current(FILE_PATH);

				AddListViewItems(hWndLV, 2, 6, fh_current.get_pair());
			}*/
			MainWindow mainWnd((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), const_cast<LPCTSTR>(optWndClass));
			mainWnd.run();
			options = mainWnd.getOptions();
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

std::string getFileName(HWND hWnd)
{

	TCHAR szFile[255] = { 0 };

	OPENFILENAME opf;
	ZeroMemory(&opf, sizeof(opf));
	opf.lStructSize = sizeof(opf);
	opf.hwndOwner = hWnd;
	opf.lpstrFile = szFile;
	opf.nMaxFile = sizeof(szFile);
	opf.lpstrFilter = ("csv\0*.csv\0");
	opf.nFilterIndex = 2;
	opf.lpstrFileTitle = NULL;
	opf.nMaxFileTitle = 0;
	opf.lpstrInitialDir = NULL;
	opf.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	return GetOpenFileName(&opf) == TRUE ? opf.lpstrFile : "";
}