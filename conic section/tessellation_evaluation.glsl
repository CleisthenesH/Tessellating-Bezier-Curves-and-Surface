// Copyright 2024 Kieran W Harvie. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.

#version 430
layout(isolines) in;
layout (location = 0) out vec2 tessCoord;

void main(void)
{
	tessCoord = gl_TessCoord.xy;

	gl_Position = (gl_in[0].gl_Position*(1-gl_TessCoord.x)
		+gl_in[1].gl_Position*gl_TessCoord.x)*(1-gl_TessCoord.x)
		+(gl_in[1].gl_Position*(1-gl_TessCoord.x)
		+gl_in[2].gl_Position*gl_TessCoord.x)*gl_TessCoord.x;
}