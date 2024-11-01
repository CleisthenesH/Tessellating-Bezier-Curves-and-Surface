// Copyright 2024 Kieran W Harvie. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.

#version 430
layout (vertices = 3) out;

uniform float weight;

void main(void)
{
	if(gl_InvocationID == 0)
	{
		gl_TessLevelOuter[0] = 1.0;
		gl_TessLevelOuter[1] = 128.0; 
	}

	if(gl_InvocationID == 1)
		gl_out[gl_InvocationID].gl_Position = vec4(gl_in[gl_InvocationID].gl_Position.xyz,weight);
	else
		gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}