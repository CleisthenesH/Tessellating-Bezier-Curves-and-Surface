// Copyright 2024 Kieran W Harvie. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.

#include <SDL.h>
#include <gl/glew.h>
#include <stdio.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

SDL_Window* window = NULL;

GLuint VAO = 0;
GLuint VBO = 0;

GLuint program = 0;

int init_sdl()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0 || 
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4) < 0 ||
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,3) < 0 ||
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) < 0)
	{
		fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
		return 1;
	}

	window = SDL_CreateWindow(
		"bezier tesselation",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
	);

	if (!window)
	{
		fprintf(stderr, "could not create window: %s\n", SDL_GetError());
		return 1;
	}

	SDL_GLContext context = SDL_GL_CreateContext(window);

	if (!context)
	{
		printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
		return 1;
	}

	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();

	if (glewError != GLEW_OK)
	{
		printf("Error initializing GLEW! %s\n", glewGetErrorString(glewError));
		return 1;
	}

	if (SDL_GL_SetSwapInterval(1) < 0)
		printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

	return 0;
}

int attach_shader(const char* file, unsigned long int type)
{
	FILE* stream;

	fopen_s(&stream, file, "rb");

	if (!stream)
	{
		printf("Unable to read: %s\n", file);
		return 1;
	}

	fseek(stream, 0L, SEEK_END);
	unsigned long fileSize = ftell(stream);
	fseek(stream, 0L, SEEK_SET);

	char* contents = malloc(fileSize + 1);

	if (!contents)
	{
		printf("Unable to allocate memory to read: %s\n", file);
		fclose(stream);

		return 1;
	}

	const size_t size = fread(contents, 1, fileSize, stream);
	contents[size] = 0; 

	fclose(stream);

	GLuint shader = glCreateShader(type);

	glShaderSource(shader, 1, (GLchar * []) { contents }, NULL);
	glCompileShader(shader);

	GLint shader_status = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_status);

	if (shader_status != GL_TRUE)
	{
		printf("Unable to compile %s %d!\n", file, shader);
		return 1;
	}

	glAttachShader(program, shader);

	free(contents);
}

int init_program()
{
	program = glCreateProgram();

	if (attach_shader("vertex.glsl", GL_VERTEX_SHADER) ||
		attach_shader("fragment.glsl", GL_FRAGMENT_SHADER) ||
		attach_shader("tessellation_evaluation.glsl", GL_TESS_EVALUATION_SHADER))
	{
		return 1;
	}

	glLinkProgram(program);

	GLint program_link_status = GL_TRUE;
	glGetProgramiv(program, GL_LINK_STATUS, &program_link_status);
	if (program_link_status != GL_TRUE)
	{
		printf("Error linking program %d!\n", program);

		GLint max_len = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_len);

		GLchar* log = calloc(max_len, sizeof(GLchar));
		glGetProgramInfoLog(program, max_len, &max_len, &log[0]);
		log[max_len] = '\0';

		printf("%d\t%s\n", max_len, log);

		return 1;
	}

	glPatchParameteri(GL_PATCH_VERTICES, 10);
	glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, (GLfloat[3]) { 16, 16, 16 });
	glPatchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, (GLfloat[1]) { 5 });

	return 0;
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program);
	glDrawArrays(GL_PATCHES,0, 10);

	glUseProgram(0);
	glDrawArrays(GL_POINTS,0,10);

	SDL_GL_SwapWindow(window);
}

int main(int argc, char* args[])
{
	if (init_sdl())
		return 0;

	if (init_program())
		return 0;

	// VBO data
	//    0
	//   1,2
	//  3,4,5
	// 6,7,8,9

	GLfloat control_points[20] =
	{
		 0.000000f, 0.500000f, 
		-0.306250f, 0.275000f, 
		-0.350000f,-0.250000f, 
		-0.487500f,-0.083333f, 
		-0.4000000,-0.400000f, 
		-0.234375f,-0.429167f, 
		-0.500000f,-0.500000f, 
		-0.200000f,-0.600000f, 
		 0.200000f,-0.600000f, 
		 0.500000f,-0.500000f, 
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Create VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), NULL);
	glBufferData(GL_ARRAY_BUFFER, 2 * 10 * sizeof(GLfloat), control_points, GL_STATIC_DRAW);

	glPointSize(6);
	render();

	SDL_Event e;
	int vertex_selection = -1;

	while (SDL_WaitEvent(&e))
	{
		if (e.type == SDL_QUIT)
			break;

		if (e.type == SDL_MOUSEBUTTONUP)
			vertex_selection = -1;

		if (e.type == SDL_MOUSEBUTTONDOWN)
			for (int i = 0; i < 10; i++)
			{
				const GLint dx = SCREEN_WIDTH * (0.5f +0.5f * control_points[2 * i]) - e.button.x;
				const GLint dy = SCREEN_HEIGHT * (0.5f  - +0.5f * control_points[2 * i + 1]) - e.button.y;

				if (dx > -3 && dx < 3 && dy > -3 && dy < 3)
					vertex_selection = i;
			}

		if (e.type == SDL_MOUSEMOTION)
		{
			if (vertex_selection < 0)
				continue;

			control_points[2* vertex_selection] = 2.0f * ((float)e.button.x) / ((float)SCREEN_WIDTH) - 1.0f;
			control_points[2* vertex_selection+1] = 1.0f - 2.0f * ((float)e.button.y) / ((float)SCREEN_HEIGHT);

			glBufferData(GL_ARRAY_BUFFER, 2 * 10 * sizeof(GLfloat), control_points, GL_STATIC_DRAW);

			render();
		}
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}