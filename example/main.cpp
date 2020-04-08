
#include "win32_gl/win32_gl.h"
#include "draw.h"

struct state
{
    std::string msg;
};

LRESULT CALLBACK window_proc(HWND h_wnd, UINT message, WPARAM w_param, LPARAM l_param)
{
    state *s = nullptr;

    if (message == WM_CREATE)
    {
        CREATESTRUCT *create_struct = (CREATESTRUCT*)l_param;
        s = (state*)create_struct->lpCreateParams;
        SetWindowLongPtr(h_wnd, GWLP_USERDATA, (LONG_PTR)s);
    } 
    else
    {
        LONG_PTR ptr = GetWindowLongPtr(h_wnd, GWLP_USERDATA);
        s = reinterpret_cast<state*>(ptr);
    }

    switch (message) {
        case WM_CLOSE:
        {
            PostQuitMessage(0);
        } break;
        case WM_SIZE:
        {
            int width = LOWORD(l_param);
            int height = HIWORD(l_param);

            handle_window_resize(width, height);

            HDC device_context = GetDC(h_wnd);
            SwapBuffers(device_context);
            ReleaseDC(h_wnd, device_context);
        } break;
        case WM_KEYUP:
        {
            if (w_param == VK_ESCAPE) 
            {
                PostQuitMessage(0);
            }
            else
            {
                switch (w_param)
                {
                    case 'S':
                    {
                        MessageBoxA(0, s->msg.c_str(), "State retrieved", MB_ICONINFORMATION | MB_OK);
                    } break;
                }
            }
        } break;
        default:
        {
            return DefWindowProc(h_wnd, message, w_param, l_param);
        };
    }

    return 0;
}

const int width = 700;
const int height = 300;

int CALLBACK WinMain(HINSTANCE h_instance, HINSTANCE h_prev_instance, LPSTR lp_cmd_line, int n_show_cmd)
{
    auto wnd = win32_gl::window("Test Class", window_proc);

    state s{"state being handled in window callback"};

    if (!wnd.create_window(CW_USEDEFAULT, CW_USEDEFAULT, width, height, "Test Window", &s))
    {
        MessageBoxA(0, "Failed to create window", "Error", MB_ICONERROR | MB_OK | MB_ICONEXCLAMATION);
        return 1;
    }

    if (!wnd.init_opengl(4, 5))
    {
        MessageBoxA(0, "Failed to initialise OpenGL", "Error", MB_ICONERROR | MB_OK | MB_ICONEXCLAMATION);
        return 1;
    }

    wnd.show();

    if (!init_draw(width, height))
    {
        MessageBoxA(0, "Failed to load OpenGL functions", "Error", MB_ICONERROR | MB_OK | MB_ICONEXCLAMATION);
        return 1;
    }

    if (!wnd.set_window_title(get_gpu_info().c_str()))
    {
        MessageBoxA(0, "Failed to set window title", "Error", MB_ICONERROR | MB_OK | MB_ICONEXCLAMATION);
        return 1;
    }

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
        
        SwapBuffers(wnd.device_context);
    }

    return 0;
}