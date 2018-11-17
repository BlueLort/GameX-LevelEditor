#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec4 homgSpace;
out vec3 Normal;
out vec2 TexCoords;
out float visibility;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
	
	
	Normal=transpose(inverse(mat3(model))) * aNormal;
	TexCoords=aTexCoords;
	FragPos=vec3( model * vec4(aPos, 1.0));
	homgSpace=projection * view * model * vec4(aPos, 1.0);
	gl_Position = homgSpace;

}