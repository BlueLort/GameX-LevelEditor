#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

const vec4 CUTPLANE_UP=vec4(0,-1,0,0.1);
const vec4 CUTPLANE_DOWN=vec4(0,1,0,0.1);
void main()
{
    TexCoords = aPos;
     vec4 worldPOS=vec4(aPos,1.0);
    gl_ClipDistance[0]=dot(worldPOS,CUTPLANE_DOWN);
    gl_ClipDistance[1]=dot(worldPOS,CUTPLANE_UP);

    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}  