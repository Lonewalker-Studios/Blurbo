#include <glad/gl.h>        
#include <SDL3/SDL.h>
#include <Window.h>
#include <string>
#include <iostream>

void reportError() {
	std::string error = SDL_GetError();
	std::cout << "Error! " + error << std::endl;
}
int width = 800;
int height = 600;
std::string title = "Blurbo";

int main() {
	bool running{ true };
	SDL_InitFlags sdlFlags = SDL_INIT_VIDEO | SDL_INIT_AUDIO;

	
	if (!SDL_Init(sdlFlags)) {
		reportError();
		return 1;
	}

	// Setup OpenGL 
	if (!SDL_GL_LoadLibrary(NULL)) {
		reportError();
		return 1;
	}

	// Set OpenGL attributes
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

	// Set number of bits per channel
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	// Create window
	SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
	blurbo_window::Window window(title, width, height, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, true, flags);
	if (!window.getWindow()) {
		reportError();
		return 1;
	}

	// Create OpenGL context
	window.setGLContext(SDL_GL_CreateContext(window.getWindow().get()));
	if (!window.getGLContext()) {
		reportError();
		return 1;
	}

	// Make the context current
	SDL_GL_MakeCurrent(window.getWindow().get(), window.getGLContext());
	SDL_GL_SetSwapInterval(1);

	// Initialize GLAD 
	if (gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress) == 0) {
		reportError();
		return 1;
	}

	SDL_Event event{};

	// Window loop
	while (running) {
		// Process events
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_EVENT_QUIT:
				running = false;
				break;
			case SDL_EVENT_KEY_DOWN:
				if (event.key.down == SDLK_ESCAPE) {
					running = false;
					break;
			default:
				break;
				}
			}
		}
		glViewport(window.getX(), window.getY(), window.getWidth(width), window.getHeight(height));
		glClearColor(0.f, 0.f, 1.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		SDL_GL_SwapWindow(window.getWindow().get());
	}

	return 0;
}