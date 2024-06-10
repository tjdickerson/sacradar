//
// Created by tjdic on 6/9/2024.
//

#include <chrono>
#include <windows.h>

#include "core/SacRadar.h"
#include "core/mapping/SacMap.h"
#include "win_opengl.h"
#include "win_support.h"

#define WND_CLASS_NAME "SACRadar"

bool g_is_running = true;
SacRadar sac_radar;

HWND             init_window(HINSTANCE);
LRESULT CALLBACK win_message_callback(HWND, UINT, WPARAM, LPARAM);
LRESULT          application_message_callback(HWND, UINT, WPARAM, LPARAM);


int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show) {
    HWND hwnd = init_window(instance);

    if (!hwnd) {
        log_message("Failed to initialize window.");
        return -1;
    }

    ShowWindow(hwnd, cmd_show);
    UpdateWindow(hwnd);

    HDC hdc = GetDC(hwnd);
    MSG message;

    auto s_time = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::high_resolution_clock::now() - s_time;
    while (g_is_running) {
        GetMessage(&message, nullptr, 0, 0);
        TranslateMessage(&message);
        DispatchMessage(&message);

        long long delta_ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
        sac_radar.update(delta_ms);

        SwapBuffers(hdc);
        elapsed = std::chrono::high_resolution_clock::now() - s_time;
        s_time = std::chrono::high_resolution_clock::now();
    }

    // cleanup

    return (int)message.wParam;
}

HWND init_window(HINSTANCE instance) {
    WNDCLASS wnd_class      = {};
    wnd_class.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wnd_class.lpfnWndProc   = win_message_callback;
    wnd_class.hInstance     = instance;
    wnd_class.lpszClassName = WND_CLASS_NAME;
    wnd_class.hIcon         = nullptr;
    wnd_class.hCursor       = LoadCursor(nullptr, IDC_ARROW);

    if (!RegisterClass(&wnd_class)) {
        log_message("Failed to register window class.");
        return (HWND)1;
    }

    HWND hwnd = CreateWindowEx(0,
                               wnd_class.lpszClassName,
                               "sacradar",
                               WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                               CW_USEDEFAULT,
                               CW_USEDEFAULT,
                               CW_USEDEFAULT,
                               CW_USEDEFAULT,
                               nullptr,
                               nullptr,
                               instance,
                               nullptr);

    if (!hwnd) {
        log_message("Failed to get window handle from CreateWindowEx.");
        return (HWND)2;
    }

    return hwnd;
}

LRESULT CALLBACK win_message_callback(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) {
    LRESULT result       = 0;
    bool    pass_message = true;

    if (pass_message) {
        result = application_message_callback(hwnd, message, w_param, l_param);
    }

    return result;
}

LRESULT application_message_callback(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) {
    LRESULT result = 0;

    // Pass message to ImGui
    // result = ImGui_ImplWin32_WndProcHandler(hwnd, message, w_param, l_param);

    switch (message) {

    case WM_CREATE: {
        HDC t_hdc = GetDC(hwnd);
        result    = win_init_opengl(t_hdc);

        const auto *cs = (CREATESTRUCT *)l_param;

        // Context initialized, now set up the core application
        sac_radar = {};
        sac_radar.init(hwnd);
        sac_radar.update_viewport(cs->cx, cs->cy);

        g_is_running = true;
    } break;

    case WM_CLOSE: {
        g_is_running = false;
        PostQuitMessage(0);
    } break;

    default:
        result = DefWindowProc(hwnd, message, w_param, l_param);
        break;
    }

    return result;
}
