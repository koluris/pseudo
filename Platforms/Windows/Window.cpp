#include "Global.h"


HWND hWnd;
HINSTANCE hInst;

LRESULT CALLBACK WndProc(HWND hWnd, UINT mes, WPARAM wParam, LPARAM lParam) {
    switch(mes) {
        case WM_CREATE:
            OutputDebugString("WM_CREATE\n");
            return 0;
            
        case WM_COMMAND:
            OutputDebugString("WM_COMMAND\n");
            switch (LOWORD(wParam)) {
            case ID_ACTION_OPEN:
                break;

            case ID_ACTION_BIOSSHELL:
                break;

            case ID_ACTION_EXIT:
                break;

            case ID_HELP_ABOUTPSEUDO:
                break;
            }
            return 0;
            
        case WM_CLOSE:
            OutputDebugString("WM_CLOSE\n");
            return 0;
            
        case WM_DESTROY:
            OutputDebugString("WM_DESTROY\n");
            return 0;
    }
    
    return DefWindowProc(hWnd, mes, wParam, lParam);
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {
        0, WndProc, 0, 0, hInst, LoadIcon(hInst, (LPCSTR)0), LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_WINDOW + 1), MAKEINTRESOURCE(IDR_PSEUDO_MENU), "wc"
    };
    
    if (!MakeClass(&wc)) {
		MesBox(0, "MakeClass failed", "PSeudo", 0);
        
        return 1;
    }
    
    hInst = hInstance;
    
    HWND hWnd = CreateWindow("wc", "PSeudo : Alpha", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, 0, 0, hInstance, 0);
    
    if (!hWnd) {
		MesBox(0, "CreateWindow failed", "PSeudo", 0);
        
        return 1;
    }
    
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    
    MSG mes;
    while(GetMes(&mes, 0, 0, 0)) {
		TranslateMes(&mes);
		 DispatchMes(&mes);
    }
    
    return (int)mes.wParam;
}
