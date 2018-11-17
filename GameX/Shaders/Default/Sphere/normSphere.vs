#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out mat3 TBN;

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
    vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
    vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));
    // re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
    // then retrieve perpendicular vector B with the cross product of T and N
    vec3 B = cross(N, T);
      //  vec3 B = normalize(vec3(model * vec4(aBiTangent, 0.0)));
    TBN = mat3(T, B, N);

   vec4 worldPOS=vec4(FragPos,1.0);
    gl_ClipDistance[0]=dot(worldPOS,CUTPLANE_DOWN);
    gl_ClipDistance[1]=dot(worldPOS,CUTPLANE_UP);
    gl_Position = projection * view * vec4(FragPos,1.0);
   
}