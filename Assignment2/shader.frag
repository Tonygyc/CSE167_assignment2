
/*

#version 330 core
// This is a sample fragment shader.

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.
in float sampleExtraOutput;
in vec3 normalVector;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 color;

void main()
{
    //input the normals from the vertex shader, output the color
    color = vec4(normalize(normalVector), 1.0f);
    
}

*/

// BELOW TAKEN AND MODIFIED DIRECTLY FROM PROVIDED LINK ON ASSIGNMENT WRITEUP

#version 330 core
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    
    int turnedOn;
};

struct DirLight {
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    int turnedOn;
};

struct PointLight {
    vec3 position;
    
    //float constant;
    float linear;
    //float quadratic;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    int turnedOn;
};

struct SpotLight {
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    vec3 directionCone;
    float cutoff;
    float exponent;
    
    float quadratic;
    
    int turnedOn;
};

#define NR_POINT_LIGHTS 4

in vec3 FragPos;
in vec3 Normal;
//in vec2 TexCoords;

out vec4 color;

uniform vec3 viewPos;
uniform DirLight dirLight;
//uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform Material material;
uniform SpotLight spotLight;
uniform int shaderMode;
uniform PointLight pointLight;

// Function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    // Properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 result = vec3(0.0f, 0.0f, 0.0f);
    
    // == ======================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == ======================================
    // Phase 1: Directional lighting
    if (dirLight.turnedOn == 1)
        result += CalcDirLight(dirLight, norm, viewDir);
    // Phase 2: Point lights
    if (pointLight.turnedOn == 1)
        result += CalcPointLight(pointLight, norm, FragPos, viewDir);
    // Phase 3: Spot light
    if (spotLight.turnedOn == 1)
        result += CalcSpotLight(spotLight, norm, FragPos, viewDir);
    
    if (shaderMode == 0) {
        color = vec4(result, 1.0f);
    }
    else if (shaderMode == 1){
        color = vec4(norm, 1.0f);
    }
}

// Calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.direction);
    //if (FragPos.z < 0) {lightDir = normalize(-light.direction);}
  
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // Combine results
    vec3 ambient = light.ambient * material.ambient;//vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * material.diffuse;//vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * material.specular * dot(material.specular, light.specular);//vec3(texture(material.specular, TexCoords));
    return (ambient + diffuse + specular);
}

// Calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.linear * distance);
    // Combine results
    vec3 ambient = light.ambient * material.ambient;//vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * material.diffuse;//vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * material.specular;//vec3(texture(material.specular, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
   // if (light.position.z < fragPos.z) {
   //    return ambient;//vec3(0.0f, 0.0f, 0.0f);
   //}
    //else {
        return (ambient + diffuse + specular);
   // }
}

// Calculates the color when using a spot light
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.quadratic * (distance * distance));
    float spotLightFactor = pow(dot(lightDir, normalize(light.directionCone)), light.exponent);
    // Combine results
    vec3 ambient = light.ambient * material.ambient;//vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * material.diffuse;//vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * material.specular;//vec3(texture(material.specular, TexCoords));
    ambient *= (attenuation * spotLightFactor);
    diffuse *= (attenuation * spotLightFactor);
    specular *= (attenuation * spotLightFactor);
    
    if (dot(-lightDir,normalize(light.directionCone)) <= cos(light.cutoff)) {
        return ambient;//vec3(0.0f, 0.0f, 0.0f);
    }
    else if (light.position.z < fragPos.z) {
        return vec3(0.0f, 0.0f, 0.0f);
    }
    else {
        return (ambient + diffuse + specular);
    }
}

