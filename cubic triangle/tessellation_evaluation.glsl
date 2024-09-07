// Copyright 2024 Kieran W Harvie. All rights reserved.
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file.

#version 430
layout (triangles) in;
layout (location = 0) out vec3 barycoordinates;

//    0
//   1,2
//  3,4,5
// 6,7,8,9

void main(void)
{
    vec2 control_points[10];

    for(int i = 0; i < 10; i++)
        control_points[i] = gl_in[i].gl_Position.xy;

    for(int i = 3; i>0; i--)
    for(int j = 0; j<i; j++)
    for(int k = 0; k<=j; k++)
    {
        int idx = j*(j+1)/2+k;

        control_points[idx] = control_points[idx]*gl_TessCoord.x
            +control_points[idx+j+1]*gl_TessCoord.y
            +control_points[idx+j+2]*gl_TessCoord.z;
    }

    gl_Position = vec4(control_points[0],0,1);

    barycoordinates = gl_TessCoord;
}