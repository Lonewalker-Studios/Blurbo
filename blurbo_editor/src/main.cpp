#include <glad/gl.h>
#include <SDL3/SDL.h>
#include <Window.h>
#include <string>
#include <iostream>
#include <SOIL2.h>
#include <glm/glm.hpp>

struct UVs {
	float u, v, width, height;
	UVs() : u{ 0.f }, v{ 0.f }, width{0.f}, height{0.f} {

	}
};

bool loadTexture(const std::string& path, int& width, int& height, bool blended) {
	int channels = 0;
	unsigned char* image = SOIL_load_image(path.c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);
	if (!image) {
		std::cout << "Error loading texture! " + path + " " + SOIL_last_result() << std::endl;
		return false;
	}
	GLint format = GL_RGBA;
	switch (channels) {
	case 1:
		format = GL_RED;
		break;
	case 2:
		format = GL_RG;
		break;
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		break;
	default:
		break;
	}
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	if (!blended) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
	return true;
}

void reportError() {
	std::string error = SDL_GetError();
	std::cout << "Error! " + error << std::endl;
}

bool checkShader(GLuint shader, const char* name) {
	int ok = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
	if (!ok) {
		char log[512];
		glGetShaderInfoLog(shader, 512, nullptr, log);
		std::cout << name << " shader error: " << log << std::endl;
	}
	return ok != 0;
}

bool checkProgram(GLuint program) {
	int ok = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &ok);
	if (!ok) {
		char log[512];
		glGetProgramInfoLog(program, 512, nullptr, log);
		std::cout << "Program link error: " << log << std::endl;
	}
	return ok != 0;
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

	if (!SDL_GL_LoadLibrary(NULL)) {
		reportError();
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
	blurbo_window::Window window(title, width, height, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, true, flags);
	if (!window.getWindow()) {
		reportError();
		return 1;
	}

	window.setGLContext(SDL_GL_CreateContext(window.getWindow().get()));
	if (!window.getGLContext()) {
		reportError();
		return 1;
	}

	SDL_GL_MakeCurrent(window.getWindow().get(), window.getGLContext());
	SDL_GL_SetSwapInterval(1);

	if (gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress) == 0) {
		reportError();
		return 1;
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	int texWidth{ 0 }, texHeight{ 0 };
	if (!loadTexture("Content/textures/bake.png", texWidth, texHeight, false)) {
		return 1;
	}

	float vertices[] = {
		-0.5f,  0.5f, 0.0f, 0.f, 0.f,
		 0.5f,  0.5f, 0.0f, 1.f, 0.f,
		 0.5f, -0.5f, 0.0f, 1.f, 1.f,
		-0.5f, -0.5f, 0.0f, 0.f, 1.f
	};
	GLuint indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	const char* vertexSource = "#version 460 compatibility\n"
		"layout (location = 0) in vec3 aPos;\n"
		"layout (location = 1) in vec2 aTexCoords;\n"
		"out vec2 fragUVs;\n"
		"void main()\n"
		"{\n"
		"gl_Position = vec4(aPos, 1.0);\n"
		"fragUVs = aTexCoords;\n"
		"}\n";

	const char* fragSource = "#version 460 compatibility\n"
		"out vec4 color;\n"
		"in vec2 fragUVs;\n"
		"uniform sampler2D uTexture;\n"
		"void main()\n"
		"{\n"
		"color = texture(uTexture, fragUVs);\n"
		"}\n";

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	bool vertexOk = checkShader(vertexShader, "Vertex");

	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fragSource, NULL);
	glCompileShader(fragShader);
	bool fragmentOk = checkShader(fragShader, "Fragment");

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragShader);
	glLinkProgram(shaderProgram);
	bool linkOk = checkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);

	if (!vertexOk || !fragmentOk || !linkOk) {
		return 1;
	}

	glUseProgram(shaderProgram);
	glUniform1i(glGetUniformLocation(shaderProgram, "uTexture"), 0);

	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(sizeof(float) * 3));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	SDL_Event event{};

	while (running) {
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

		int viewWidth, viewHeight;
		SDL_GetWindowSizeInPixels(window.getWindow().get(), &viewWidth, &viewHeight);
		glViewport(0, 0, viewWidth, viewHeight);

		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		SDL_GL_SwapWindow(window.getWindow().get());
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteTextures(1, &textureID);
	glDeleteProgram(shaderProgram);

	return 0;
}