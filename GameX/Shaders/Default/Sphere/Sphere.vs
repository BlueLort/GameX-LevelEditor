#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const vec4 CUTPLANE_UP=vec4(0,-1,0,0.1);
const vec4 CUTPLANE_DOWN=vec4(0,1,0,0.1);

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal=transpose(inverse(mat3(model))) * aNormal;
    TexCoord = aTexCoord;
    
  vec4 worldPOS=vec4(FragPos,1.0);
    gl_ClipDistance[0]=dot(worldPOS,CUTPLANE_DOWN);
    gl_ClipDistance[1]=dot(worldPOS,CUTPLANE_UP);
    gl_Position = projection * view * vec4(FragPos,1.0) ;
   
}