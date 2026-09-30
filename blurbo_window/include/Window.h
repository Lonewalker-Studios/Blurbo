#pragma once
#include <SDL_Wrappers.h>
#include <string>
#include <memory>

namespace blurbo_window {
	using WindowPtr = std::shared_ptr<Window>;

	class Window {
	private:
		WindowPtr windowPtr;
		SDL_GLContext glContext;
		std::string title;
		int width, height;
		Uint32 windowFlags;
		void createWindow(Uint32 windowFlags);
	public:
		Window() : Window("Test Game", 800, 600, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, true, NULL) {

		}
		Window(const std::string title, int width, int height, int x, int y, bool vSync = true, Uint32 windowFlags = (SDL_WINDOW_OPENGL));
		~Window();
		inline void setGLContext(SDL_GLContext& glContext) { this->glContext = glContext; }
		inline SDL_GLContext getGLContext() const { return glContext; }
		inline WindowPtr& getWindow() { return windowPtr; }

	};
}