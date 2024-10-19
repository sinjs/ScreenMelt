// ReSharper disable CppLocalVariableMayBeConst
// ReSharper disable CppParameterMayBeConst
//   The Windows API sometimes requires variables to not be constant which breaks the compiler

#include <iostream>
#include <windows.h>

// The window class name
static const std::wstring class_name = L"ScreenMelt";

// Screen width and height
static int width;
static int height;

// Window proc function for the main window
LRESULT CALLBACK WndProc(HWND window_handle, UINT message, WPARAM param_w, LPARAM param_l) {
    switch (message) {
        // When the window gets created (opened)
        case WM_CREATE: {
            // Get screen and window DCs
            HDC source_dc = GetDC(nullptr);
            HDC window_dc = GetDC(window_handle);

            // Copy global DC contents to window DC
            BitBlt(window_dc, 0, 0, width, height, source_dc, 0, 0, SRCCOPY);

            // Release both DCs
            ReleaseDC(window_handle, window_dc);
            ReleaseDC(nullptr, source_dc);

            // Set timer for every melting tick
            SetTimer(window_handle, 0, 100, nullptr);

            // Show the window
            ShowWindow(window_handle, SW_SHOW);
            break;
        }

        // When the window gets destroyed (closed)
        case WM_DESTROY: {
            // Kill the melting timer
            KillTimer(window_handle, 0);

            // Post the quit message with exit code 0
            PostQuitMessage(0);
            break;
        }

        // When the window gets painted
        case WM_PAINT: {
            // Validate the update region (entire window)
            ValidateRect(window_handle, nullptr);
            break;
        }

        // Every 100 milliseconds
        case WM_TIMER: {
            // Get window DC
            HDC dc = GetDC(window_handle);

            // Get coordinates for the BitBlt operation
            const int x = rand() % width - 75;
            const int y = rand() % 15;
            const int cx = rand() % 150;
            const int cy = height;

            // Copy window DC contents to itself, only the specified coordinates for the melting effect
            BitBlt(dc, x, y, cx, cy, dc, x, 0, SRCCOPY);

            // Release DC
            ReleaseDC(window_handle, dc);
            break;
        }

        default:
            break;
    }

    // Define window proc function
    return DefWindowProcW(window_handle, message, param_w, param_l);
};

// Print the last Win32 error into `std::wcerr`
inline void PrintError() {
    wchar_t buf[256];

    // Format the message and write output into buf
    FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   nullptr, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                   buf, (sizeof(buf) / sizeof(wchar_t)), nullptr);

    // Write buffer to wide standard error
    std::wcerr << buf << std::endl;
}

// Windows main entrypoint function
int APIENTRY WinMain(HINSTANCE instance_handle, HINSTANCE, PSTR, int) {
    // Set the global variables to the screen width and height
    width = GetSystemMetrics(SM_CXSCREEN);
    height = GetSystemMetrics(SM_CYSCREEN);

    // Create the window class
    WNDCLASSW window_class;
    window_class.hInstance = instance_handle;
    window_class.hCursor = LoadCursorW(nullptr, reinterpret_cast<LPCWSTR>(IDC_ARROW));
    window_class.lpszClassName = class_name.c_str();
    window_class.lpfnWndProc = WndProc;
    window_class.style = 0;
    window_class.hIcon = nullptr;
    window_class.cbClsExtra = 0;
    window_class.hbrBackground = nullptr;

    // Register the window class
    if (!RegisterClassW(&window_class)) {
        PrintError();
        return 1;
    }

    // Create the window
    if (!CreateWindowExW(
        WS_EX_TOPMOST,
        class_name.c_str(),
        nullptr,
        WS_POPUP,
        0,
        0,
        width,
        height,
        nullptr,
        nullptr,
        instance_handle,
        nullptr)) {
        PrintError();
        return 1;
    }

    // Get the number of seconds since system startup and use that as the random seed
    const DWORD tick_count = GetTickCount();
    srand(tick_count);

    // The last received message
    MSG message;

    // If the last message is not the quit message, peek the current message and write it to message
    while (message.message != WM_QUIT) {
        if (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }

    // Return exit code 0 after WM_QUIT was dispatched
    return 0;
}
