// velocity-cli.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <Shellapi.h>
#include <tchar.h>
#include <Psapi.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

// keyboard hooks
HHOOK hHook = NULL;
KBDLLHOOKSTRUCT kbdStruct;

// window stuff
NOTIFYICONDATA nid;
HWND hwnd;

// Function Declarations
void InitNotifyIconData();
void RegisterClass();
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT __stdcall KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0)
    {
        // Get the keyboard event data
        kbdStruct = *((KBDLLHOOKSTRUCT*)lParam);

        // Check if the key combination is Ctrl + K
        if (wParam == WM_KEYDOWN && (GetAsyncKeyState(VK_CONTROL) & 0x8000) && kbdStruct.vkCode == 'K')
        {
            // Get the handle to the active window
            HWND hWnd = GetForegroundWindow();

            // Get the process ID of the active window
            DWORD processID;
            GetWindowThreadProcessId(hWnd, &processID);

            // Open the process
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

            // Get the name of the executable file for the process
            TCHAR fileName[MAX_PATH];
            GetModuleFileNameEx(hProcess, NULL, fileName, MAX_PATH);
            TCHAR* executableName = PathFindFileName(fileName);

            // Compare the application name, and skip execution if the image is Linear or Notion
            if (_tcscmp(_T("Linear.exe"), executableName) == 0 || _tcscmp(_T("Notion.exe"), executableName) == 0 ||  _tcscmp(_T("brave.exe"), executableName) == 0)
            {
                
                std::cout << "Ctrl + K pressed (no action)" << std::endl;
                std::wcout << "Image name: " << executableName << std::endl;
                // Return without taking any action if the active window is OtherApp
                return CallNextHookEx(hHook, nCode, wParam, lParam);
            }

            // If our window is already in foreground, hide it
            if (hWnd == hwnd) {
                ShowWindow(hwnd, SW_HIDE);
                return CallNextHookEx(hHook, nCode, wParam, lParam);

            }

            // Your code here to handle the keyboard event

            std::cout << "Ctrl + K pressed" << std::endl;
            std::wcout << "Image name: " << executableName << std::endl;
            ShowWindow(hwnd, SW_RESTORE);
            SetForegroundWindow(hwnd);

        }
        else if (wParam == WM_KEYDOWN && (GetAsyncKeyState(VK_CONTROL) & 0x8000) && kbdStruct.vkCode == 'C')
        {
            // Check if current window is in focus
            // Get the handle to the active window
            HWND hWnd = GetForegroundWindow();

            // Get the process ID of the active window
            DWORD processID;
            GetWindowThreadProcessId(hWnd, &processID);

            // Open the process
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);

            // Get the name of the executable file for the process
            TCHAR fileName[MAX_PATH];
            GetModuleFileNameEx(hProcess, NULL, fileName, MAX_PATH);
            TCHAR* executableName = PathFindFileName(fileName);

            if (_tcscmp(_T("velocity-cli.exe"), executableName) == 0 || _tcscmp(_T("WindowsTerminal.exe"), executableName) == 0) {
                // Unhook the hook
                UnhookWindowsHookEx(hHook);
                std::cout << "Unhooked the hook!" << std::endl;

                // Post a WM_QUIT message to exit the message loop
                PostQuitMessage(0);
            }

        }
    }

    return CallNextHookEx(hHook, nCode, wParam, lParam);
}



// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file



// Global Variables


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    // Register the window class
    RegisterClass();

    // Create the window
    hwnd = CreateWindowEx(0, L"MyWindowClass", L"My Window", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL);

    // Initialize the NOTIFYICONDATA structure
    InitNotifyIconData();

    // Show the window
    ShowWindow(hwnd, nCmdShow);
    // Set the hook
    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);

    // Run the message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Unhook the hook if it's still set
    if (hHook)
    {
        UnhookWindowsHookEx(hHook);
        std::cout << "Unhooked the hook!" << std::endl;

    }


    return 0;

}

// Function Definitions
void InitNotifyIconData()
{
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hwnd;
    nid.uID = 100;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_USER + 1;
    nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcscpy_s(nid.szTip, L"My App");
    Shell_NotifyIcon(NIM_ADD, &nid);
}

void RegisterClass()
{
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = L"MyWindowClass";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
    {
        MessageBoxA(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        exit(EXIT_FAILURE);
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_USER + 1:
        switch (lParam)
        {
        case WM_LBUTTONDOWN:
            ShowWindow(hwnd, SW_RESTORE);
            break;
        case WM_RBUTTONDOWN:
            // Show context menu here
            break;
        }
        break;
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
        {
            ShowWindow(hwnd, SW_HIDE);
        }
        break;
    case WM_DESTROY:
        Shell_NotifyIcon(NIM_DELETE, &nid);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}