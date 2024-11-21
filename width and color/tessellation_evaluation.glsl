// Copyright 2024 Kieran W Harvie. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.

#version 430
layout(quads) in;
layout (location = 0) out vec3 color;

layout(location = 0) in struct VertexAttrib
{
	vec2 pos;
	float width;
	vec3 color;
} in_data[];

vec2 tangent()
{
	vec2 buff[3];

	for(int i=0;i<3;i++)
		buff[i] = in_data[i+1].pos - in_data[i].pos;

	for(int i = 2; i >= 0; i--)
	for(int j = 0; j < i; j++)
		buff[j] = mix(buff[j],buff[j+1],gl_TessCoord.x);

	return buff[0];
}

VertexAttrib vertexMix()
{
	VertexAttrib buff[4];

	for(int i = 0; i < 4; i++)
		buff[i] = in_data[i];

	for(int i = 3; i >= 0; i--)
	for(int j = 0; j < i; j++)
	{
		buff[j].pos = mix(buff[j].pos,buff[j+1].pos,gl_TessCoord.x);
		buff[j].width = mix(buff[j].width,buff[j+1].width,gl_TessCoord.x);
		buff[j].color = mix(buff[j].color,buff[j+1].color,gl_TessCoord.x);
	}

	return buff[0];
}

void main(void)
{
	VertexAttrib v = vertexMix();

	vec2 t = tangent();

	// There's multiple ways to combine these values for different effects, for example:
	// Remove the normalize and the line will get wider as it moves quicker.
	// Dynamically change the gl_TessCoord.y factor to dynamically change how the line is centered.
	// Mix an attribute with other functions of gl_TessCoord.x.

	vec2 n = v.width*(2*gl_TessCoord.y-1)*normalize(vec2(-t.y,t.x));

	gl_Position = vec4(v.pos+n,0,1);
	color = v.color;
}