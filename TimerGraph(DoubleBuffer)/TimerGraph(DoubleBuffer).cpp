#define _CRT_SECURE_NO_WARNINGS
#include "pch.h"

#include "CQueue.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
HINSTANCE g_hInstance;
LPCWSTR lpszClass = L"DoubleBuffer";

CQueue<int> g_Que;
int g_iHeight;

//-------------------------------------
// 전역에 메모리DC 핸들 보관
//-------------------------------------
HDC g_hMemDC;
HBITMAP g_hMemBitmap;
HBITMAP g_hMemBitmapOld;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrecInstance,
    LPSTR lpszCmdParam, int nCmdShow)
{
    HWND hWnd;
    MSG Message;
    WNDCLASS wndClass;

    // 클래스 설정
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndClass.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wndClass.hIcon = LoadIconW(NULL, IDI_APPLICATION);
    wndClass.hInstance = hInstance;
    wndClass.lpfnWndProc = (WNDPROC)WndProc;
    wndClass.lpszClassName = lpszClass;
    wndClass.lpszMenuName = NULL;
    wndClass.style = CS_HREDRAW | CS_VREDRAW;

    // 윈도우 클래스 등록
    RegisterClassW(&wndClass);

    // CreateWindow
    hWnd = CreateWindowW
    (
        lpszClass,
        L"Timer Graph(Double Buffer)",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        (HMENU)NULL,
        hInstance,
        NULL
    );

    // 만들어진 윈도우를 화면에 출력
    ShowWindow(hWnd, nCmdShow);

    // 메시지 루프
    while (GetMessageW(&Message, NULL, 0, 0))
    {
        TranslateMessage(&Message);
        DispatchMessageW(&Message);
    }

    return (int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect;
    GetClientRect(hWnd, &rect);

    HPEN hPen;
    HPEN hOldPen;

    static int iOldX;
    static int iOldY;
    static int iCurX;
    static int iCurY;

    switch (iMessage)
    {
    case WM_CREATE:
        SetTimer(hWnd, 1, 1, NULL);
        
        hdc = GetDC(hWnd);
        g_hMemDC = CreateCompatibleDC(hdc);

        if (g_hMemBitmap == NULL)
        {
            g_hMemBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
        }

        g_hMemBitmapOld = (HBITMAP)SelectObject(g_hMemDC, g_hMemBitmap);

        ReleaseDC(hWnd, hdc);
        break;
    case WM_TIMER:

        if (!g_Que.EnQueue(g_iHeight))
        {
            g_Que.DeQueue();
        }
        g_iHeight = rect.bottom - (rand() % 200);

        // 클라이언트 영역 셋팅
        InvalidateRect(hWnd, NULL, TRUE);

        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);

        // 윈도우 클라이언트 영역사에서의 좌표값 반환한다.
        GetClientRect(hWnd, &rect);

        // 클라이언트 증가방향 설정          // 가변 매핑모드
        SetMapMode(g_hMemDC, MM_ISOTROPIC);// 사용자 정의 모드 아래 함수들로 사용자가 증가 방향을 설정 할 수 있다

        // 우 하단좌표(rect.right, rect.bottom) 설정
        SetWindowExtEx(g_hMemDC, rect.right, rect.bottom, NULL);

        SetViewportExtEx(g_hMemDC, rect.right, rect.bottom, NULL);

        iOldX = 0;
        iOldY = g_Que.GetFrontValue();
        iCurX = iOldX;
        iCurY = iOldY;

        hPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 255));
        hOldPen = (HPEN)SelectObject(g_hMemDC, hPen);

        g_Que.InitTemp();
        while (g_Que.Peek(&iOldY))
        {
            MoveToEx(g_hMemDC, iCurX * 10 /*rect.right / 100*/, iCurY, NULL);
            LineTo(g_hMemDC, iOldX * 10 /*rect.right / 100*/, iOldY);

            iCurX = iOldX;
            iCurY = iOldY;

            iOldX++;
        }
        DeleteObject(SelectObject(g_hMemDC, hOldPen));

        //GetClientRect(hWnd, &rect);

        // hdc와 g_hMemDC 합치기
        BitBlt(hdc, 0, 0, rect.right, rect.bottom, g_hMemDC, 0, 0, SRCCOPY);

        // 설정한 패턴으로 지우기
        PatBlt(g_hMemDC, 0, 0, rect.right, rect.bottom, WHITENESS);//BLACKNESS

        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        KillTimer(hWnd, 1);

        // 오브젝트 정리
        SelectObject(g_hMemDC, g_hMemBitmapOld);
        DeleteObject(g_hMemBitmap);
        DeleteDC(g_hMemDC);

        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcW(hWnd, iMessage, wParam, lParam);
    }

    return 0;
}