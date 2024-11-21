// Copyright 2024 Kieran W Harvie. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.

#version 430

layout(location = 0) in vec2 pos;
layout(location = 1) in float width;
layout(location = 2) in vec3 color;

layout(location = 0) out struct VertexAttrib
{
	vec2 pos;
	float width;
	vec3 color;
} out_data;

void main()
{
	out_data.pos = pos;
	out_data.width = width;
	out_data.color = color;
}