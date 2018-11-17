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


//light and material related
uniform int nr_active_plight;
uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;

//General uniforms
uniform float Time;
uniform int SCR_WIDTH;
uniform int SCR_HEIGHT;
uniform float farSpace;
uniform float nearSpace;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir,vec3 diffColor,vec3 specColor);  
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir,vec3 diffColor,vec3 specColor);  
void main(){

    vec3 diffColor= vec3(texture(material.texture_diffuse, TexCoord));
    vec3 specColor= vec3(texture(material.texture_specular, TexCoord));
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

       //  Dir light
    vec3 result = CalcDirLight(dirLight, norm,viewDir,diffColor,specColor);
    // Point lights
    for(int i = 0; i < nr_active_plight; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir,diffColor,specColor);     
    
    FragColor = vec4(result, 1.0);

}
 
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir,vec3 diffColor,vec3 specColor)
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
    return  (ambient +diffuse + specular);
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
    return (ambient + diffuse + specular);
} 