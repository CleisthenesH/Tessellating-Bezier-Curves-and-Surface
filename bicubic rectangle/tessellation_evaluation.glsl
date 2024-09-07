// Copyright 2024 Kieran W Harvie. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.

#version 430
layout(quads, equal_spacing, ccw) in;
layout (location = 0) out vec2 tessCoord;

void main(void)
{
   tessCoord = gl_TessCoord.xy;

   vec2 buff[16];

   	for (int i = 0; i < 16; i++)
		buff[i] = gl_in[i].gl_Position.xy;

	for (int i = 3; i > 0; i--)
	for (int j = 0; j < i; j++)
	for (int k = 0; k < 4; k++)
		buff[k + 4 * j] = mix(buff[k + 4 * j],buff[k + 4 * j+4],gl_TessCoord.x);

	for (int i = 3; i > 0; i--)
	for (int j = 0; j < i; j++)
		buff[j] = mix(buff[j],buff[j+1],gl_TessCoord.y);


	gl_Position = vec4(buff[0],0,1);
}