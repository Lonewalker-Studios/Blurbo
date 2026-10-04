#include <glad/gl.h>
#include <SDL3/SDL.h>
#include <Window.h>
#include <string>
#include <iostream>

void reportError() {
	std::string error = SDL_GetError();
	std::cout << "Error! " + error << std::endl;
}

void checkShader(GLuint shader, const char* name) {
	int ok = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
	if (!ok) {
		char log[512];
		glGetShaderInfoLog(shader, 512, nullptr, log);
		std::cout << name << " shader error: " << log << std::endl;
	}
}

void checkProgram(GLuint program) {
	int ok = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &ok);
	if (!ok) {
		char log[512];
		glGetProgramInfoLog(program, 512, nullptr, log);
		std::cout << "Program link error: " << log << std::endl;
	}
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

	// Temporary vertex data
	float vertices[] = {
		 0.0f,  0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f
	};

	// Temporary shader code (vertex and fragment)
	const char* vertexSource = "#version 460 compatibility\n"
		"layout (location = 0) in vec3 aPos;\n"
		"void main()\n"
		"{\n"
		"gl_Position = vec4(aPos, 1.0);\n"
		"}\n";

	const char* fragSource = "#version 460 compatibility\n"
		"out vec4 color;\n"
		"void main()\n"
		"{\n"
		"color = vec4(1.0, 1.0, 1.0, 1.0);\n"
		"}\n";

	// Compile vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	checkShader(vertexShader, "Vertex");

	// Compile fragment shader
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fragSource, NULL);
	glCompileShader(fragShader);
	checkShader(fragShader, "Fragment");

	// Link shader program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragShader);
	glLinkProgram(shaderProgram);
	checkProgram(shaderProgram);

	
	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);

	
	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

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
				if (event.key.key == SDLK_ESCAPE) {
					running = false;
				}
				break;
			default:
				break;
			}
		}

		
		int width, height;
		SDL_GetWindowSizeInPixels(window.getWindow().get(), &width, &height);
		glViewport(0, 0, width, height);

		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		SDL_GL_SwapWindow(window.getWindow().get());
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);

	return 0;
}