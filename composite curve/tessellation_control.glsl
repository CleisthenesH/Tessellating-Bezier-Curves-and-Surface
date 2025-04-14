// Copyright 2025 Kieran W Harvie. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.

#version 430
layout (vertices = 4) out;

struct pair_vertex
{
	vec2 p0, p1;
};

layout(location = 0) in pair_vertex[] vert_in;

vec2 control_points()
{
	switch(gl_InvocationID)
	{
		case 0:
		return vert_in[0].p0;
		case 1:
		return vert_in[0].p1;
		case 2:
		return 2.0*vert_in[1].p0-vert_in[1].p1;
		default:
		return vert_in[1].p0;
	}
}

void main(void)
{
	if(gl_InvocationID == 0)
	{
		gl_TessLevelOuter[0] = 1.0;
		gl_TessLevelOuter[1] = 128.0; 
	}

	gl_out[gl_InvocationID].gl_Position = vec4(control_points(),0,1);
}