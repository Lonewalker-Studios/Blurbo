#include "Window.h"
#include <iostream>
#include <string>
namespace blurbo_window {
	Window::Window(const std::string title, int width, int height, int x, int y, bool vSync, Uint32 windowFlags)
		: windowPtr{ nullptr }, glContext{}, title{title}, width{width}, height{height}, x{x}, y{y}, windowFlags{windowFlags}
	{
		createWindow(windowFlags);
		// Enable VSync
		if (vSync) {
			if (!SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1")) {
				std::cout << "Failed to enable VSync!" << std::endl;
			}
			std::cout << "Window created!" << std::endl;
		}
	}

	Window::~Window()
	{
	}
	void Window::setWindowTitle(const std::string& title) {
		this->title = title;
		if (windowPtr) {
			SDL_SetWindowTitle(windowPtr.get(), this->title.c_str());
		}
	}
	void Window::createWindow(Uint32 windowFlags) {
		windowPtr = std::shared_ptr<SDL_Window>(SDL_CreateWindow(title.c_str(), x, y, width, height, windowFlags), SDL_DestroyWindow);
	}
}
