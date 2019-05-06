#include "MainWndClass.h"

constexpr int GRAPH_BUTTON_ID = 3001;
constexpr int OPTIONS_BUTTON_ID = 3002;
constexpr int IDC_LISTVIEW_ID_1 = 3003;
constexpr int IDC_LISTVIEW_ID_2 = 3004;
constexpr LPCTSTR optWndClass = "OptWnd";

static std::string FILE_PATH;
static std::vector<std::string> options;

static const char* header[] = { "Num", "Value" };

MainWndClass::MainWndClass(HINSTANCE hInstance, LPCTSTR className) : WndClass{hInstance, className}
{
	tAPP.reset(new TApplication("App", 0, 0));

	hWnd = CreateWindow(className, TEXT("Peltier Regress"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 500, 400, NULL, NULL,
		hInstance, this);

	hGraphButton = CreateWindow("BUTTON", "Graph",
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_DISABLED, 400, 300, 80, 20,
		hWnd, (HMENU)GRAPH_BUTTON_ID, hInstance, NULL);

	hBrowseButton = CreateWindow("BUTTON", "Options",
		WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 400, 330, 80, 20,
		hWnd, (HMENU)OPTIONS_BUTTON_ID, hInstance, NULL);
}

int MainWndClass::run()
{
	INITCOMMONCONTROLSEX icex;
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&icex);

	Matrix_Plan::fill_table("C:\\AppData\\Fisher_table.csv");

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

MainWndClass::~MainWndClass()
{
}

LRESULT MainWndClass::thisWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_COMMAND:
		switch (wParam)
		{
			// Обработка нажатия на кнопку Graph
		case GRAPH_BUTTON_ID:
		{
			FileHandler fh_current(options.at(0));
			Matrix_Plan mtrx_current{ 3, options };
			mtrx_current.read_from_file(fh_current);
			mtrx_current.count();
			mtrx_current.assessment();
			Matrix_Plan mtrx_voltage{ 3, options };
			FileHandler fh_voltage(options.at(1));
			mtrx_voltage.read_from_file(fh_voltage);
			mtrx_voltage.count();
			mtrx_voltage.assessment();

			FunctionHandler *hFnc = new FunctionHandler{ {mtrx_current, mtrx_voltage}, options };
			hFnc->initialize_graphs(tAPP);
		}
		break;
		// Обработка нажатия на кнопку Options
		case OPTIONS_BUTTON_ID:
		{
			OptionsWnd optWnd((HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), const_cast<LPCTSTR>(optWndClass));
			optWnd.run();
			options = optWnd.getOptions();

			HWND childWnd = FindWindowEx(hWnd, NULL, "BUTTON", NULL);
			if (!options.empty())
			{
				while (HWND hDelWnd = FindWindowEx(hWnd, NULL, WC_LISTVIEW, NULL))
					DestroyWindow(hDelWnd);


				try
				{
					// Заполнение ListView
					HWND hWndLV_1 = CreateWindow(WC_LISTVIEW, "", WS_CHILD | LVS_REPORT | WS_VISIBLE,
						10, 10, 200, 200, hWnd, (HMENU)IDC_LISTVIEW_ID_1, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
					HWND hWndLV_2 = CreateWindow(WC_LISTVIEW, "", WS_CHILD | LVS_REPORT | WS_VISIBLE,
						250, 10, 200, 200, hWnd, (HMENU)IDC_LISTVIEW_ID_2, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);

					FileHandler fh_listView1{ options.at(0) };
					FileHandler fh_listView2{ options.at(1) };

					ListView_SetExtendedListViewStyleEx(hWndLV_1, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
					ListView_SetExtendedListViewStyleEx(hWndLV_2, 0, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
					SetListViewColumns(hWndLV_1, 2, 6, const_cast<char**>(header));
					SetListViewColumns(hWndLV_2, 2, 6, const_cast<char**>(header));
					AddListViewItems(hWndLV_1, 2, 6, fh_listView1.get_pair());
					AddListViewItems(hWndLV_2, 2, 6, fh_listView2.get_pair());

					EnableWindow(childWnd, TRUE);
				}
				catch (std::runtime_error& err)
				{
					MessageBox(hWnd, err.what(), "Error", NULL);
					EnableWindow(childWnd, FALSE);
				}
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
		PostQuitMessage(0);
	}
	break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int MainWndClass::SetListViewColumns(HWND hWndLV, int colNum, int textMaxLen, char ** header)
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

BOOL __stdcall MainWndClass::AddListViewItems(HWND hWndLV, int colNum, int textMaxLen, const std::vector<std::pair<int, double>> num_value)
{
	LVITEM lvI;

	lvI.pszText = LPSTR_TEXTCALLBACK;
	lvI.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE;
	lvI.stateMask = 0;
	lvI.iSubItem = 0;
	lvI.state = 0;

	for (int index = 0; index < num_value.size(); index++)
	{
		lvI.iItem = index;
		lvI.iImage = index;

		char first_val[3];
		char second_val[15];

		_itoa_s(num_value[index].first, first_val, 10);
		sprintf_s(second_val, "%f", num_value[index].second);


		if (ListView_InsertItem(hWndLV, &lvI) == -1)
			return FALSE;
		ListView_SetItemText(hWndLV, index, 0, first_val);
		ListView_SetItemText(hWndLV, index, 1, second_val);
	}

	return TRUE;
}