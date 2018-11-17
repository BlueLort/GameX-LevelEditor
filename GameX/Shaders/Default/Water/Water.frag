#version 330 core

//ALL SPECULAR LIGHTS ARE COMMENTED I DONT CALCULATE THEM
//i did what felt okay for me xD
out vec4 FragColor;
struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define NR_POINT_LIGHTS 100//max 100 POINT LIGHTS IN SCENE  


in vec4 homgSpace;
in vec3 Normal;//NOT USED YOU MAY TRY TO USE IT :)
                //its (x,y,z)=0,1,0 instead i used the normal map its here to show that its imported
in vec2 TexCoords;
in vec3 FragPos;

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D depthReflection;
uniform sampler2D depthRefraction;

uniform sampler2D dudvMap;
uniform sampler2D normalMap;

uniform int nr_active_plight;
uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];


//General uniforms
uniform float Time;
uniform int SCR_WIDTH;
uniform int SCR_HEIGHT;
uniform float farSpace;
uniform float nearSpace;

vec4 matDiffuseColor;
const float DISTORT_FACTOR=0.025;
const float TILING_FACTOR=12;


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);  
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);  
void main()
{
    vec4 norm_map_vals=(texture(dudvMap,vec2(TexCoords.x+Time,TexCoords.y)*TILING_FACTOR)*2.0)-1.0;
    vec3 norm=normalize(vec3(norm_map_vals.r,norm_map_vals.b,norm_map_vals.g));


	//SET CAMERA DIR

	vec3  FragToCamDir=normalize(viewPos-FragPos);
	//Calculate mix factor for reflect and refactor effect
	float MixFactor=dot(FragToCamDir,vec3(0,1,0));
	////////


	vec2 offset=((texture(dudvMap,vec2(TexCoords.x+Time,TexCoords.y)*TILING_FACTOR).rg)*2-1.0)*DISTORT_FACTOR;
	vec2 ndc=((homgSpace.xy/homgSpace.w)/2+0.5);
	
	//ADD OFFSET AND FIX OFFSCREEEN TEX COORDS
	vec2 refLTexCoord=vec2(ndc.x,-ndc.y)+offset/2;
	refLTexCoord.x=clamp(refLTexCoord.x,0.001,0.995);
	refLTexCoord.y=clamp(refLTexCoord.y,-0.995,-0.001);
	vec2 refRTexCoord=ndc+offset;
	refRTexCoord=clamp(refRTexCoord,0.001,0.995);
	

	vec4 reflectionColor=texture(reflectionTexture,refLTexCoord);
	vec4 refractionColor=texture(refractionTexture,refRTexCoord);
   	FragColor = mix(reflectionColor,refractionColor,MixFactor);
	FragColor=mix(FragColor,vec4(0,0.1,0.7,1.0),0.2);

	 

	matDiffuseColor=FragColor;
  vec3 result =vec3(matDiffuseColor);
   

    // phase 1: Directional lighting
    result += CalcDirLight(dirLight, norm,  FragToCamDir);
    // phase 2: Point lights
    for(int i = 0; i < nr_active_plight; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos,  FragToCamDir);    


   	FragColor = vec4(result,1.0);
}
 vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading  BLINN PHONG
    vec3 halfWayDir =normalize(lightDir+viewDir);
    float spec = pow(max(dot(normal, halfWayDir), 0.0), 32.0);
    // combine results
    vec3 ambient  = light.ambient *vec3(matDiffuseColor);
    vec3 diffuse  = light.diffuse  * (diff/3) *vec3(matDiffuseColor);
    vec3 specular = light.specular * spec *0.6 ;
    return  (ambient + diffuse + specular);
}  
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading  BLINN PHONG
    vec3 halfWayDir =normalize(lightDir+viewDir);
    float spec = pow(max(dot(normal, halfWayDir), 0.0), 32.0);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient *vec3(matDiffuseColor) ;
    vec3 diffuse  = light.diffuse  * (diff/3) *vec3(matDiffuseColor) ;
    vec3 specular = light.specular * spec*0.6 ;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
} 
