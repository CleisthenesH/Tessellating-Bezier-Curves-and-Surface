// Copyright 2025 Kieran W Harvie. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.

#version 430

layout(location = 0) in vec2 p0;
layout(location = 1) in vec2 p1;

struct pair_vertex
{
	vec2 p0, p1;
};

layout(location = 0) out pair_vertex vert_out;

void main()
{
	vert_out.p0 = p0;
	vert_out.p1 = p1;
}