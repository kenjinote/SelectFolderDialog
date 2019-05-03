#include<windows.h>
#include<shlobj.h>

#define ID_BUTTON1 100

TCHAR szClassName[] = TEXT("Window");

int CALLBACK BrowseCallbackProc(HWND hWnd, UINT msg, LPARAM lParam, LPARAM lpData)
{
	if (msg == BFFM_INITIALIZED)
	{
		// https://connect.microsoft.com/VisualStudio/feedback/details/518103/bffm-setselection-does-not-work-with-shbrowseforfolder-on-windows-7#
		SendMessage(hWnd, BFFM_SETSELECTION, 1, lpData);
		Sleep(1000);
		PostMessage(hWnd, BFFM_SETSELECTION, 1, lpData);
	}
	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hEdit;
	static TCHAR szDirectoryPath[MAX_PATH];
	static TCHAR szDirectoryName[MAX_PATH];
	switch (msg)
	{
	case WM_CREATE:
		hEdit = CreateWindowEx(WS_EX_CLIENTEDGE,
			TEXT("EDIT"),
			0,
			WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
			10, 10, 256, 30,
			hWnd,
			0,
			((LPCREATESTRUCT)lParam)->hInstance,
			0
			);
		CreateWindow(
			TEXT("BUTTON"),
			TEXT("..."),
			WS_CHILD | WS_VISIBLE,
			276, 10, 30, 30,
			hWnd,
			(HMENU)ID_BUTTON1,
			((LPCREATESTRUCT)lParam)->hInstance,
			0
			);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_BUTTON1:
			GetWindowText(hEdit, szDirectoryPath, MAX_PATH);
			BROWSEINFO  BrowseInfo = {
				hWnd,
				0,
				szDirectoryName,
				TEXT("フォルダを選択してください。"),
				BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_NONEWFOLDERBUTTON,
				&BrowseCallbackProc,
				(LPARAM)szDirectoryPath,
				0
			};
			LPITEMIDLIST pidl = (LPITEMIDLIST)SHBrowseForFolder(&BrowseInfo);
			LPMALLOC pMalloc = 0;
			if (pidl != NULL&&SHGetMalloc(&pMalloc) != E_FAIL)
			{
				SHGetPathFromIDList(pidl, szDirectoryPath);
				SetWindowText(hEdit, szDirectoryPath);
				pMalloc->Free(pidl);
				pMalloc->Release();
			}
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("Window"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
		);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
