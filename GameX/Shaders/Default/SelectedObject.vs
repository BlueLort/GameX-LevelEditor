#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
const vec4 CUTPLANE_UP=vec4(0,-1,0,0.1);
const vec4 CUTPLANE_DOWN=vec4(0,1,0,0.1);
void main()
{
     vec4 worldPOS=model * vec4(aPos, 1.0);
    gl_ClipDistance[0]=dot(worldPOS,CUTPLANE_DOWN);
    gl_ClipDistance[1]=dot(worldPOS,CUTPLANE_UP);
    gl_Position = projection * view *worldPOS;
   
}