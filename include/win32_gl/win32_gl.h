#pragma once

#include <windows.h>
#include <gl\GL.h>
#include "gl\glext.h"
#include "gl\wglext.h"

namespace win32_gl
{

typedef LRESULT (*window_proc)(HWND h_wnd, UINT message, WPARAM w_param, LPARAM l_param);

class window
{
public:
    window(char *class_name, window_proc callback);
    ~window();

    bool create_window(const int px, const int py, const int width, const int height, const char *window_title, void *state);
    void resize_window(int width, int height);
    bool init_opengl(const int major, const int minor);
    bool set_window_title(const char *window_title);
    void show();

    HWND h_wnd;
    HDC device_context;
    HGLRC render_context;
private:
    char *class_name;
    HMODULE h_module;
    window_proc callback;
};

window::window(char *class_name, window_proc callback) : class_name(class_name), callback(callback)
{
}

window::~window()
{
    if (render_context)
    {
        wglDeleteContext(render_context);
    }
    
    ReleaseDC(h_wnd, device_context);
    DestroyWindow(h_wnd);
}

bool window::create_window(const int px, const int py, const int width, const int height, const char *window_title, void *state = nullptr)
{
    // Back on 16 bit windows HMODULE and HINSTANCE were not the same thing
    h_module = NULL;
    GetModuleHandleEx(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
        (LPCTSTR)this,
        &h_module);

    WNDCLASSEXA wcex = {};
    wcex.cbSize = sizeof(wcex);
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS ;
    wcex.lpfnWndProc = callback;
    wcex.hInstance = h_module;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.lpszClassName = class_name;

    if (!RegisterClassExA(&wcex))
    {
        return false;
    }

    RECT rect = {};
    rect.bottom = height;
    rect.right = width;

    AdjustWindowRectEx(&rect, WS_TILED, false, 0);

    h_wnd = CreateWindowExA(
        0,
        class_name,
        window_title,
        WS_OVERLAPPEDWINDOW,
        px, py, 
        rect.right - rect.left, rect.bottom - rect.top,
        NULL,
        NULL,
        h_module,
        state
        );

    if (h_wnd == nullptr)
    {
        return false;
    }

    return true;
}

void window::resize_window(int width, int height)
{
    RECT size = {};
    size.bottom = height;
    size.right = width;

    AdjustWindowRectEx(&size, WS_TILED, false, 0);

    RECT pos = {};
    GetWindowRect(h_wnd, &pos);

    SetWindowPos(
        h_wnd,
        0,
        pos.left, pos.top,
        size.right - size.left, size.bottom - size.top,
        SWP_ASYNCWINDOWPOS
        );
}

bool window::init_opengl(const int major, const int minor)
{
    HWND tmp_window = CreateWindowA(
        class_name, "Temporary Window",
        WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        0, 0,
        1, 1,
        NULL, NULL,
        h_module, NULL);

    HDC tmp_window_context = GetDC(tmp_window);

    PIXELFORMATDESCRIPTOR tmp_pixel_format_desc = {};
    tmp_pixel_format_desc.nSize = sizeof(tmp_pixel_format_desc);
    tmp_pixel_format_desc.nVersion = 1;
    tmp_pixel_format_desc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    tmp_pixel_format_desc.iPixelType = PFD_TYPE_RGBA;
    tmp_pixel_format_desc.cColorBits = 24;
    tmp_pixel_format_desc.cAlphaBits = 8;
    tmp_pixel_format_desc.cDepthBits = 24;
    
    int tmp_pixel_format_desc_id = ChoosePixelFormat(tmp_window_context, &tmp_pixel_format_desc);

    if (tmp_pixel_format_desc_id == 0) 
    {
        return false;
    }

    // Not sure if this is needed but invoked anyway, as it seems to add more data to struct
    if (!DescribePixelFormat(tmp_window_context, tmp_pixel_format_desc_id, 
                             tmp_pixel_format_desc.nSize, &tmp_pixel_format_desc))
    {
        return false;
    }

    if (!SetPixelFormat(tmp_window_context, tmp_pixel_format_desc_id, &tmp_pixel_format_desc))
    {
        return false;
    }

    HGLRC tmp_render_context = wglCreateContext(tmp_window_context);

    if (tmp_render_context == nullptr) 
    {
        return false;
    }

    wglMakeCurrent(tmp_window_context, tmp_render_context);

    auto wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)(wglGetProcAddress("wglChoosePixelFormatARB"));

    if (wglChoosePixelFormatARB == nullptr) 
    {
        return false;
    }
    
    auto wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)(wglGetProcAddress("wglCreateContextAttribsARB"));

    if (wglCreateContextAttribsARB == nullptr) 
    {
        return false;
    }

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(tmp_render_context);
    ReleaseDC(tmp_window, tmp_window_context);
    DestroyWindow(tmp_window);

    /*
    Create the OpenGL context we wil be taking forward
    */
    device_context = GetDC(h_wnd);

    const int pixel_attribs[] = 
    {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
        WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
        WGL_COLOR_BITS_ARB, 32,
        WGL_ALPHA_BITS_ARB, 8,
        WGL_DEPTH_BITS_ARB, 24,
        WGL_STENCIL_BITS_ARB, 8,
        WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
        WGL_SAMPLES_ARB, 4,
        0
    };
    
    int pixel_format_ID; 
    UINT num_formats;

    bool status = wglChoosePixelFormatARB(
        device_context, pixel_attribs, 
        NULL, 1, 
        &pixel_format_ID, &num_formats);
    
    if (status == false || num_formats == 0) 
    {
        return false;
    }

    PIXELFORMATDESCRIPTOR pixel_Format_Desc;
    DescribePixelFormat(device_context, pixel_format_ID, sizeof(pixel_Format_Desc), &pixel_Format_Desc);
    SetPixelFormat(device_context, pixel_format_ID, &pixel_Format_Desc);

    int  context_attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, major,
        WGL_CONTEXT_MINOR_VERSION_ARB, minor,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };
    
    render_context = wglCreateContextAttribsARB(device_context, 0, context_attribs);
    
    if (render_context == nullptr) 
    {
        return false;
    }

    if (!wglMakeCurrent(device_context, render_context)) 
    {
        return false;
    }

    return true;
}

bool window::set_window_title(const char *window_title)
{
    return SetWindowTextA(h_wnd, window_title);
}

void window::show()
{
    ShowWindow(h_wnd, SW_SHOW);
}

};
