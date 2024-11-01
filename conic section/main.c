// Copyright 2024 Kieran W Harvie. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.

#include <SDL.h>
#include <gl/glew.h>
#include <stdio.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT SCREEN_WIDTH

SDL_Window* window = NULL;

GLuint VAO = 0;
GLuint VBO = 0;
GLuint IBO = 0;

GLuint program = 0;

GLfloat control_point[12] = {
	-0.3,0.0, //P_0
	 0.0,0.3, //P_1
	 0.3,0.0, //P_2
	-0.4,0.1, //T_0
	 0.4,0.1, //T_2
	 0.0,0.5  //B
};

GLfloat weight = 0.75;

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

	return 0;
}

int init_program()
{
	program = glCreateProgram();

	if (
		attach_shader("vertex.glsl", GL_VERTEX_SHADER) ||
		attach_shader("fragment.glsl", GL_FRAGMENT_SHADER) ||
		attach_shader("tessellation_evaluation.glsl", GL_TESS_EVALUATION_SHADER) ||
		attach_shader("tessellation_control.glsl", GL_TESS_CONTROL_SHADER))
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

	glPatchParameteri(GL_PATCH_VERTICES, 3);

	return 0;
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program);
	glDrawElements(GL_PATCHES, 3, GL_UNSIGNED_INT, NULL);

	glUseProgram(0);
	glDrawArrays(GL_POINTS, 0, 6);

	SDL_GL_SwapWindow(window);
}

static GLfloat signed_area(size_t i, size_t j, size_t z)
{
	return control_point[2 * i] * control_point[2 * j + 1]
		- control_point[2 * j] * control_point[2 * i + 1]
		+ control_point[2 * z] * control_point[2 * i + 1]
		- control_point[2 * i] * control_point[2 * z + 1]
		+ control_point[2 * j] * control_point[2 * z + 1]
		- control_point[2 * z] * control_point[2 * j + 1];
}

void geometry()
{
	// Get intersection
	const GLfloat l0 = control_point[7] - control_point[1];
	const GLfloat l1 = control_point[0] - control_point[6];
	const GLfloat l2 = control_point[7] * control_point[0] - control_point[6] * control_point[1];

	const GLfloat l3 = control_point[9] - control_point[5];
	const GLfloat l4 = control_point[4] - control_point[8];
	const GLfloat l5 = control_point[9] * control_point[4] - control_point[8] * control_point[5];

	GLfloat d = l0 * l4 - l1 * l3;

	if (d == 0)
	{
		// TODO: handle parallel case
		return;
	}

	d = 1.0 / d;

	control_point[10] = d * (l2 * l4 - l1 * l5);
	control_point[11] = d * (l0 * l5 - l2 * l3);

	// Calculate area-coordinates
	const GLfloat b0 = signed_area(1, 5, 2);
	const GLfloat b1 = signed_area(0, 1, 2);
	const GLfloat b2 = signed_area(0, 5, 1);

	// Calculate weight and control point
	weight = 0.5 * b1 / sqrtf(b0 * b2);

	if (signed_area(0, 5, 2) < 0)
		weight *= -1;

	control_point[10] *= weight;
	control_point[11] *= weight;

	// Pass to shader
	glUseProgram(program);
	GLint uniform_location = glGetUniformLocation(program, "weight");
	glUniform1f(uniform_location, weight);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), control_point, GL_STATIC_DRAW);
}

int main(int argc, char* args[])
{
	if (init_sdl())
		return 0;

	if (init_program())
		return 0;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), control_point, GL_STATIC_DRAW);
	
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		3*sizeof(GLuint),
		(GLuint[3]){0,5,2}, GL_STATIC_DRAW);

	glPointSize(6);
	geometry();
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
			for (int i = 0; i < 5; i++)
			{
				const GLint dx = SCREEN_WIDTH * (0.5f +0.5f * control_point[2 * i]) - e.button.x;
				const GLint dy = SCREEN_HEIGHT * (0.5f  - +0.5f * control_point[2 * i + 1]) - e.button.y;

				if (dx > -3 && dx < 3 && dy > -3 && dy < 3)
					vertex_selection = i;
			}

		if (e.type == SDL_MOUSEMOTION)
		{
			if (vertex_selection < 0)
				continue;

			control_point[2* vertex_selection] = 2.0f * ((float)e.button.x) / ((float)SCREEN_WIDTH) - 1.0f;
			control_point[2* vertex_selection+1] = 1.0f - 2.0f * ((float)e.button.y) / ((float)SCREEN_HEIGHT);

			geometry();
			render();
		}
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}