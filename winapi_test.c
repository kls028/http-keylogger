#include <windows.h>
#include <stdio.h>

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT *key = (KBDLLHOOKSTRUCT *)lParam;

        if (key->vkCode == VK_F12) {
            printf("f12 clicked\n");
            PostQuitMessage(0);
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main(void) {
    HHOOK hook = SetWindowsHookEx(
        WH_KEYBOARD_LL,
        KeyboardProc,
        GetModuleHandle(NULL),
        0
    );

    if (hook == NULL) {
        printf("error: %lu\n", GetLastError());
        return 1;
    }

    printf("test active, press f12 to end\n");

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {

    }

    UnhookWindowsHookEx(hook);
    return 0;
}