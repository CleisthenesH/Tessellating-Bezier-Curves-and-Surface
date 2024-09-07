// Copyright 2024 Kieran W Harvie. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.

#version 430

layout(location = 0) in vec2 position;

void main()
{
	gl_Position = vec4( position, 0, 1 );
}