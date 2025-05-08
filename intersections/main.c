// Copyright 2024-2025 Kieran W Harvie. All rights reserved.
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

GLuint program = 0;

GLfloat control_point[18] = {
	// curve
	-0.5, -0.0,
	-0.2,  0.4,
	 0.2, -0.4,
	 0.5,  0.0,

	// (possible) intersections
	 2.0,  2.0,
	 2.0,  2.0,
	 2.0,  2.0,

	// x-axis
	-2.0,  0.0,
	 2.0,  0.0
};

int init_sdl()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0 ||
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4) < 0 ||
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3) < 0 ||
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

	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, (GLfloat[2]) { 1.0, 128.0 });

	return 0;
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program);
	glDrawArrays(GL_PATCHES, 0, 4);

	glUseProgram(0);
	glDrawArrays(GL_POINTS, 0, 7);
	glDrawArrays(GL_LINES, 7, 2);

	SDL_GL_SwapWindow(window);
}

void bezier(GLfloat intersections[3])
{
	float buffer[6];

	for (int i = 0; i < 3; i++)
	{
		GLfloat t = intersections[i];
		if (t < 0 || t > 1)
		{
			control_point[8 + i * 2] = 2;
			control_point[9 + i * 2] = 2;

			continue;
		}

		buffer[0] = (1 - t) * control_point[0] + t * control_point[2];
		buffer[1] = (1 - t) * control_point[1] + t * control_point[3];
		buffer[2] = (1 - t) * control_point[2] + t * control_point[4];
		buffer[3] = (1 - t) * control_point[3] + t * control_point[5];
		buffer[4] = (1 - t) * control_point[4] + t * control_point[6];
		buffer[5] = (1 - t) * control_point[5] + t * control_point[7];

		buffer[0] = (1 - t) * buffer[0] + t * buffer[2];
		buffer[1] = (1 - t) * buffer[1] + t * buffer[3];
		buffer[2] = (1 - t) * buffer[2] + t * buffer[4];
		buffer[3] = (1 - t) * buffer[3] + t * buffer[5];

		control_point[8+i*2] = (1 - t) * buffer[0] + t * buffer[2];
		control_point[9+i*2] = (1 - t) * buffer[1] + t * buffer[3];
	}
}

int discard(GLfloat a[4])
{
	return (((a[0] < 0) ^ (a[1] > 0)) || (a[0] == 0) || (a[1] == 0))
		&& (((a[2] < 0) ^ (a[1] > 0)) || (a[2] == 0) || (a[1] == 0))
		&& (((a[2] < 0) ^ (a[3] > 0)) || (a[2] == 0) || (a[3] == 0));
}

void intersect(GLfloat* intersections)
{
	intersections[0] = -1;
	intersections[1] = -1;
	intersections[2] = -1;
	 
	const GLfloat a[4] =
	{
		control_point[1],
		control_point[3],
		control_point[5],
		control_point[7],
	};

	if (discard(a))
		return;

	if(a[0] == 0)
		*(intersections++) = 0;

	if (a[3] == 0)
		*(intersections++) = 1;

	// Note that the stack height is bound by the variation diminishing property since the subdivision is a piecewise linear interpolation.
	struct stack_frame {
		GLfloat low, high;
		GLfloat p[4];
	} stack_base[2], *stack = stack_base;

	GLfloat low = 0.0;
	GLfloat high = 1.0;

	GLfloat b[7];

	b[0] = a[0];
	b[2] = a[1];
	b[4] = a[2];
	b[6] = a[3];

	for (int i = 0; i < 1000; i++)
	{
		while (high - low < 0.0001)
		{
			*(intersections++) = low;

			if (stack < stack_base)
				return;

			low = (--stack)->low;
			high = stack->high;

			b[0] = stack->p[0];
			b[2] = stack->p[1];
			b[4] = stack->p[2];
			b[6] = stack->p[3];
		}

		b[1] = 0.5 * (b[0] + b[2]);
		b[3] = 0.5 * (b[2] + b[4]);
		b[5] = 0.5 * (b[4] + b[6]);

		b[2] = 0.5 * (b[1] + b[3]);
		b[4] = 0.5 * (b[3] + b[5]);

		b[3] = 0.5 * (b[2] + b[4]);

		if (b[3] == 0)
			*(intersections++) = 0.5 * (low + high);

		const int discard_low = discard(b);
		const int discard_high = discard(b + 3);

		if (discard_low ^ discard_high)
		{
			if (discard_high)
			{
				b[6] = b[3];
				b[4] = b[2];
				b[2] = b[1];

				high = 0.5 * (low + high);
			}
			else
			{
				b[0] = b[3];
				b[2] = b[4];
				b[4] = b[5];

				low = 0.5 * (low + high);
			}

			continue;
		}

		if (discard_high)
		{
			if (stack < stack_base)
				return;

			low = (--stack)->low;
			high = stack->high;

			b[0] = stack->p[0];
			b[2] = stack->p[1];
			b[4] = stack->p[2];
			b[6] = stack->p[3];

			continue;
		}

		const GLfloat mid = 0.5 * (low + high);
		*(stack++) = (struct stack_frame){ .low = low, .high = mid,.p = {b[0],b[1],b[2],b[3]} };

		b[0] = b[3];
		b[2] = b[4];
		b[4] = b[5];

		low = mid;
	}

	*intersections = 0.5 * (low + high);

	return;
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

	glPointSize(6);
	
	GLfloat intersections[3];
	intersect(intersections);
	bezier(intersections);

	glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), control_point, GL_STATIC_DRAW);
	
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
			for (int i = 0; i < 4; i++)
			{
				const GLint dx = SCREEN_WIDTH * (0.5f + 0.5f * control_point[2 * i]) - e.button.x;
				const GLint dy = SCREEN_HEIGHT * (0.5f - +0.5f * control_point[2 * i + 1]) - e.button.y;

				if (dx > -3 && dx < 3 && dy > -3 && dy < 3)
					vertex_selection = i;
			}

		if (e.type == SDL_MOUSEMOTION)
		{
			if (vertex_selection < 0)
				continue;

			control_point[2 * vertex_selection] = 2.0f * ((float)e.button.x) / ((float)SCREEN_WIDTH) - 1.0f;
			control_point[2 * vertex_selection + 1] = 1.0f - 2.0f * ((float)e.button.y) / ((float)SCREEN_HEIGHT);

			intersect(intersections);
			bezier(intersections);

			glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(GLfloat), control_point, GL_STATIC_DRAW);
			render();
		}
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}