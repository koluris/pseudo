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
        0, WndProc, 0, 0, hInst, LoadIcon(hInst, (LPCSTR)0), LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_WINDOW+1), 0, "wc"
    };
    
    if (!MakeClass(&wc)) {
		MesBox(0, "MakeClass failed", "PSeudo", 0);
        
        return 1;
    }
    
    hInst = hInstance;
    
    HWND hWnd = CreateWindow("wc", "PSeudoU+2122 : Alpha", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 500, 100, 0, 0, hInstance, 0);
    
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
