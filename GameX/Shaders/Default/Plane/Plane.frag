#version 330 core
out vec4 FragColor;
struct Material {
    sampler2D texture_diffuse;
    sampler2D texture_normal;
    sampler2D texture_specular;
    float shininess;
}; 
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

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoord;
in mat3 TBN;
in vec4 FragPosLightSpace;

//light and material related
uniform int nr_active_plight;
uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;
uniform sampler2D shadowDirMap;

//General uniforms
uniform float Time;
uniform int SCR_WIDTH;
uniform int SCR_HEIGHT;
uniform float farSpace;
uniform float nearSpace;




vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir,vec3 diffColor,vec3 specColor,float shadow);  
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir,vec3 diffColor,vec3 specColor);  
float ShadowCalculation(vec4 fragPosLightSpace);
void main(){
 
    vec3 diffColor= vec3(texture(material.texture_diffuse, TexCoord));
    vec3 specColor= vec3(texture(material.texture_specular, TexCoord));
    // properties
    // obtain normal from normal map in range [0,1]
    vec3 norm = texture(material.texture_normal, TexCoord).rgb;
    // transform normal vector to range [-1,1]
    norm = normalize(norm * 2.0 - 1.0);  // this normal is in tangent space
    norm = normalize(TBN*norm);

    vec3 viewDir = normalize(viewPos - FragPos);
    float shadow = ShadowCalculation(FragPosLightSpace);  
    //  Dir light
    vec3 result = CalcDirLight(dirLight, norm,viewDir,diffColor,specColor,shadow);
    // Point lights
    for(int i = 0; i < nr_active_plight; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir,diffColor,specColor);      
    
    FragColor = vec4(result, 1.0);
}
 
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir,vec3 diffColor,vec3 specColor,float shadow)
{
   
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading  BLINN PHONG
    vec3 halfWayDir =normalize(lightDir+viewDir);
    float spec = pow(max(dot(normal, halfWayDir), 0.0), 2*material.shininess);
    // combine results
    vec3 ambient  = light.ambient  *diffColor;
    vec3 diffuse  = light.diffuse  * diff * diffColor;
    vec3 specular = light.specular * spec *  specColor; 
    return (ambient + (1.0-shadow)*(diffuse + specular));
}  
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir,vec3 diffColor,vec3 specColor)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading  BLINN PHONG
    vec3 halfWayDir =normalize(lightDir+viewDir);
    float spec = pow(max(dot(normal, halfWayDir), 0.0), 2*material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  *diffColor;
    vec3 diffuse  = light.diffuse  * diff * diffColor;
    vec3 specular = light.specular * spec *  specColor; 
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient +diffuse + specular);
} 
float ShadowCalculation(vec4 fragPosLightSpace){
     vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;// presp. division( a bit meaningless as ortho gives w of 1)
     projCoords = projCoords * 0.5 + 0.5; 
     float closestDepth = texture(shadowDirMap, projCoords.xy).r; 
     float currentDepth = projCoords.z;
    if(projCoords.z > 1.0)
        return 0.0;
    //float bias = 0.005;//No Artifacts(Shadow Acne) was found without using bias so i didnt include it
    //float shadow = currentDepth/*-bias*/  > closestDepth  ? 1.0 : 0.0;  
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowDirMap, 0);
    for(int x = -1; x <= 1; x++)
    {
     for(int y = -1; y <= 1; y++)
     {
        float pcfDepth = texture(shadowDirMap, projCoords.xy + vec2(x, y) *0.7* texelSize).r; 
        shadow += currentDepth /*- bias*/ > pcfDepth ? 1.0 : 0.0;        
     }    
    }
    shadow /= 9.0;//number from 0 to 1.0
     return shadow;
}