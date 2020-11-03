#include <windows.h>
#include <stdio.h>

#include "Resource.h"

#define ConsoleOutput OutputDebugString
#define DispatchMes   DispatchMessage
#define GetMes        GetMessage
#define MakeClass     RegisterClass
#define MesBox        MessageBox
#define TranslateMes  TranslateMessage
