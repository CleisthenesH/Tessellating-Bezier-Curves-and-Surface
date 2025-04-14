// Copyright 2024 Kieran W Harvie. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.

#version 430

layout(location = 0) in vec2 tessCoord;
layout(location = 0) out vec4 diffuseColor;

uniform float weight;

void main()
{
	diffuseColor = vec4(tessCoord.x,0,1-tessCoord.x,1.0);
}