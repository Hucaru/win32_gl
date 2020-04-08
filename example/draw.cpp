#include "draw.h"

#include "glad/glad.h"

bool init = false;

void clear_background()
{
    glClearColor(0.129f, 0.586f, 0.949f, 1.0f); // rgb(33,150,243)
    glClear(GL_COLOR_BUFFER_BIT);
}

bool init_draw(const int width, const int height)
{
    // Load opengl function ptrs
    if (!gladLoadGL())
    {
        return false;
    }

    init = true;

    glViewport(0, 0, width, height);
    clear_background();

    return true;
}

void handle_window_resize(const int width, const int height)
{
    if (!init)
    {
        return;
    }

    glViewport(0, 0, width, height);
    clear_background();
}

std::string get_gpu_info()
{
    std::string title;
    title.reserve(256);
    sprintf(&title[0], "%s %s %s", (char*)glGetString(GL_VENDOR), (char*)glGetString(GL_RENDERER), (char*)glGetString(GL_VERSION));

    return title;
}