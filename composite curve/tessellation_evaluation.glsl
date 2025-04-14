// Copyright 2024 Kieran W Harvie. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.

#version 430
layout(isolines) in;
layout (location = 0) out vec2 tessCoord;

void main(void)
{
	tessCoord = gl_TessCoord.xy;

	vec2 a[4];

	for(int i = 0; i < 4; i++)
		a[i] = gl_in[i].gl_Position.xy;

	for(int i = 3; i >= 0; i--)
	for(int j = 0; j < i; j++)
		a[j] = mix(a[j],a[j+1],gl_TessCoord.x);

	gl_Position = vec4(a[0],0,1);
}