#version 330 core

uniform vec3 col;

out vec4 fragCol;


void main()
{
	fragCol = vec4(col, 0.3);
}