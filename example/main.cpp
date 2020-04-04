#include "win32_gl/win32_gl.h"

LRESULT CALLBACK window_proc(HWND h_wnd, UINT message, WPARAM w_param, LPARAM l_param)
{
    switch (message) {
        case WM_CLOSE:
        {
            PostQuitMessage(0);
        } break;
        default:
        {
            return DefWindowProc(h_wnd, message, w_param, l_param);
        };
    }

    return 0;
}

int CALLBACK WinMain(HINSTANCE h_instance, HINSTANCE h_prev_instance, LPSTR lp_cmd_line, int n_show_cmd)
{
    auto wnd = win32_gl::window("Test Class", window_proc);

    if (!wnd.create_window(CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080, "Test Window"))
    {
        return 1;
    }

    // if (!wnd.init_opengl(3, 3))
    // {
    //     return 1;
    // }

    wnd.show();

    wnd.resize_window(1400, 900);

    if (!wnd.set_window_title("Resized Window"))
    {
        return 1;
    }

    // Load opengl function ptrs

    MSG msg = {};
    bool running = true;

    while (running)
    {
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                running = false;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }
        
        glClearColor(0.129f, 0.586f, 0.949f, 1.0f); // rgb(33,150,243)
        glClear(GL_COLOR_BUFFER_BIT);
        SwapBuffers(wnd.device_context);
    }

    return 0;
}