#pragma once

#include <string>

bool init_draw(const int width, const int height);
void handle_window_resize(const int width, const int height);
std::string get_gpu_info();