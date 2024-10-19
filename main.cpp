// ReSharper disable CppLocalVariableMayBeConst
// ReSharper disable CppParameterMayBeConst
//   The Windows API sometimes requires variables to not be constant which breaks the compiler

#include <iostream>
#include <windows.h>

static const std::wstring class_name = L"ScreenMelt";

static int width;
static int height;

LRESULT CALLBACK WndProc(HWND window_handle, UINT message, WPARAM param_w, LPARAM param_l) {
    switch (message) {
        case WM_CREATE: {
            HDC source_dc = GetDC(nullptr);
            HDC window_dc = GetDC(window_handle);
            BitBlt(window_dc, 0, 0, width, height, source_dc, 0, 0, 0xCC0020u);
            ReleaseDC(window_handle, window_dc);
            ReleaseDC(nullptr, source_dc);
            SetTimer(window_handle, 0, 100, nullptr);
            ShowWindow(window_handle, SW_SHOW);
            break;
        }

        case WM_DESTROY: {
            KillTimer(window_handle, 0);
            PostQuitMessage(0);
            break;
        }

        case WM_PAINT: {
            ValidateRect(window_handle, nullptr);
            break;
        }

        case WM_TIMER: {

            HDC dc = GetDC(window_handle);
            const int x = rand() % width - 75;
            const int y = rand() % 15;
            const int cx = rand() % 150;
            const int cy = height;
            BitBlt(dc, x, y, cx, cy, dc, x, 0, 0xCC0020u);
            ReleaseDC(window_handle, dc);
            break;
        }

        default:
            break;
    }

    return DefWindowProcW(window_handle, message, param_w, param_l);
};

inline void PrintError() {
    wchar_t buf[256];
    FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                   nullptr, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                   buf, (sizeof(buf) / sizeof(wchar_t)), nullptr);

    std::wcerr << buf << std::endl;
}

int APIENTRY WinMain(HINSTANCE instance_handle, HINSTANCE, PSTR, int) {
    width = GetSystemMetrics(SM_CXSCREEN);
    height = GetSystemMetrics(SM_CYSCREEN);

    WNDCLASSW window_class;
    window_class.hInstance = instance_handle;
    window_class.hCursor = LoadCursorW(nullptr, reinterpret_cast<LPCWSTR>(IDC_ARROW));
    window_class.lpszClassName = class_name.c_str();
    window_class.lpfnWndProc = WndProc;
    window_class.style = 0;
    window_class.hIcon = nullptr;
    window_class.cbClsExtra = 0;
    window_class.hbrBackground = nullptr;

    if (!RegisterClassW(&window_class)) {
        PrintError();
        return 1;
    }

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


    const DWORD tick_count = GetTickCount();
    srand(tick_count);

    MSG message;
    while (message.message != WM_QUIT) {
        if (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }

    return 0;
}
