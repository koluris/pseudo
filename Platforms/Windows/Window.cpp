#include "Global.h"


HWND hWnd;
HINSTANCE hInst;

LRESULT CALLBACK WndProc(HWND hWnd, UINT mes, WPARAM wParam, LPARAM lParam) {
    switch(mes) {
        case WM_CREATE:
            printf("WM_CREATE\n");
            return 0;
            
        case WM_COMMAND:
            printf("WM_COMMAND\n");
            return 0;
            
        case WM_CLOSE:
            printf("WM_CLOSE\n");
            return 0;
            
        case WM_DESTROY:
            printf("WM_DESTROY\n");
            return 0;
    }
    
    return DefWindowProc(hWnd, mes, wParam, lParam);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {
        0, WndProc, 0, 0, hInst, LoadIcon(hInst, (LPCSTR)IDI_MAIN), LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_WINDOW+1), NULL, szWindowClass
    };
    
    if (!RegisterClass(&wc)) {
        MessageBox(NULL, _T("Call to RegisterClassEx failed!"), _T("Win32 Guided Tour"), NULL);
        
        return 1;
    }
    
    hInst = hInstance;
    
    HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 100, NULL, NULL, hInstance, NULL);
    
    if (!hWnd) {
        MessageBox(NULL, _T("Call to CreateWindow failed!"), _T("Win32 Guided Tour"), NULL);
        
        return 1;
    }
    
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    
    MSG mes;
    while(GetMessage(&mes, NULL, 0, 0)) {
        TranslateMessage(&mes);
        DispatchMessage(&mes);
    }
    
    return (int)mes.wParam;
}
