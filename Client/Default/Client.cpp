﻿// Client.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Client.h"
#include "MainApp.h"
#include "GameInstance.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HWND			g_hWnd;
HINSTANCE	g_hInst;                                // 현재 인스턴스입니다.
WCHAR		szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR		szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
bool			g_bFullScreen = false;
bool			g_bNeedResizeSwapChain = false;
unsigned int	g_iWinSizeX = 1600;
unsigned int	g_iWinSizeY = 900;
_float g_fTimeDelta;

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);	
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif // _DEBUG

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.
	CMainApp*			pMainApp = nullptr;

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));

    MSG msg;
	
    // 기본 메시지 루프입니다.
	pMainApp = CMainApp::Create();
	if (nullptr == pMainApp)
		return FALSE;

	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Ready_Timer(TEXT("Timer_Default"))))
		return FALSE;
	if (FAILED(pGameInstance->Ready_Timer(TEXT("Timer_60"))))
		return FALSE;

	_double			TimerAcc = 0.0;

	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (WM_QUIT == msg.message)
				break;

			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			static constexpr _double OneFrameSec = 1.0 / 60.0;
			pGameInstance->Update_Timer(TEXT("Timer_Default"));

			TimerAcc += pGameInstance->Get_TimeDelta(TEXT("Timer_Default"));

			if (TimerAcc > OneFrameSec)
			{
				pGameInstance->Update_Timer(TEXT("Timer_60"));
				_double TimeDelta = pGameInstance->Get_TimeDelta(TEXT("Timer_60"));
#ifdef _DEBUG
				if (TimeDelta > 1.0)
					TimeDelta = OneFrameSec;
#endif
				g_fTimeDelta = (_float)TimeDelta;
				pMainApp->Tick(TimeDelta);
				pMainApp->Render();
				pMainApp->Resize_BackBuffer();
				g_bNeedResizeSwapChain = false;

				TimerAcc = 0.0;
			}
		}
	}

	RELEASE_INSTANCE(CGameInstance);

	Safe_Release(pMainApp);
    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   g_hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   RECT		rcWindow = { 0, 0, (LONG)g_iWinSizeX, (LONG)g_iWinSizeY };

   AdjustWindowRect(&rcWindow, WS_OVERLAPPEDWINDOW, FALSE);

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   g_hWnd = hWnd;

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다.
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
            EndPaint(hWnd, &ps);
        }
        break;
	case WM_SIZE:
	{
		if (wParam != SIZE_MINIMIZED && !g_bFullScreen)
		{
			RECT	rt;
			GetClientRect(hWnd, &rt);
			g_iWinSizeX = rt.right - rt.left;
			g_iWinSizeY = rt.bottom - rt.top;
		}
		g_bNeedResizeSwapChain = true;
		break;
	}
	case WM_SYSKEYDOWN:
	{
		if (wParam == VK_RETURN && (GetAsyncKeyState(VK_MENU) & 0x8000))
		{
			if (!g_bFullScreen)
			{
				RECT	rt;
				GetClientRect(hWnd, &rt);
				g_iWinSizeX = rt.right - rt.left;
				g_iWinSizeY = rt.bottom - rt.top;
			}
			g_bFullScreen = !g_bFullScreen;
			g_bNeedResizeSwapChain = true;
			return 0;
		}
		break;
	}
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
