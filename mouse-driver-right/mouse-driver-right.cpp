#include <windows.h>
#include <stdio.h>

HHOOK g_mouseHook = NULL;
volatile bool g_running = true;


LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {

    if (nCode < 0) {
        return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
    }

    if (nCode == HC_ACTION && g_running) {
        MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;


        if (pMouseStruct->flags & LLMHF_INJECTED) {
            return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
        }


        if (wParam == WM_RBUTTONDOWN || wParam == WM_RBUTTONUP) {


            PostThreadMessage(GetCurrentThreadId(),
                (wParam == WM_RBUTTONDOWN) ? WM_USER + 1 : WM_USER + 2,
                0, 0);


            return 1;
        }
    }

   
    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}


BOOL WINAPI ConsoleCtrlHandler(DWORD ctrlType) {
    if (ctrlType == CTRL_C_EVENT) {
        printf("\nShutting down...\n");
        g_running = false;
        PostQuitMessage(0);
        return TRUE;
    }
    return FALSE;
}

int main() {
    printf("=== Mouse Remapper (Right -> Left) ===\n");
;


    
    SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);

    
    g_mouseHook = SetWindowsHookEx(WH_MOUSE_LL,
        LowLevelMouseProc,
        GetModuleHandle(NULL),
        0);

    if (g_mouseHook == NULL) {
        printf("Failed to install hook: %ld\n", GetLastError());
        printf("Run as Administrator!\n");
        system("pause");
        return 1;
    }

    printf("Hook installed successfully!\n");
    printf("Right clicks will be remapped to left clicks...\n\n");

    
    MSG msg;
    while (g_running && GetMessage(&msg, NULL, 0, 0) > 0) {
       
        if (msg.message == WM_USER + 1) {
            
            INPUT input = { 0 };
            input.type = INPUT_MOUSE;
            input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
            SendInput(1, &input, sizeof(INPUT));
            printf("Right down -> Left down\n");
        }
        else if (msg.message == WM_USER + 2) {
            
            INPUT input = { 0 };
            input.type = INPUT_MOUSE;
            input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
            SendInput(1, &input, sizeof(INPUT));
            printf("Right up -> Left up\n");
        }
        else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }


    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = NULL;
    }

    printf("Remapper stopped.\n");
    return 0;
}