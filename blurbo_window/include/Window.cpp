#include "Window.h"
#include <iostream>
namespace blurbo_window {
	Window::Window(const std::string title, int width, int height, int x, int y, bool vSync, Uint32 windowFlags)
		: windowPtr{ nullptr }, glContext{}, title{title}, width{width}, height{height}, x{x}, y{y}, windowFlags{windowFlags}
	{
		createWindow(windowFlags);
		// Enable VSync
		if (vSync) {
			if (!SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1")) {
				std::cout << "Failed to enable VSync!" << std::endl''
			}
		}
	}

	Window::~Window()
	{
	}
}
