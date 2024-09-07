// Copyright 2024 Kieran W Harvie. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.

#version 430

layout(location = 0) in vec2 tessCoord;
layout(location = 0) out vec4 diffuseColor;

void main()
{
	diffuseColor = vec4(tessCoord.x*(tessCoord.y+1),
		2*(1-tessCoord.x)*tessCoord.y,
		1-tessCoord.y,1);
}