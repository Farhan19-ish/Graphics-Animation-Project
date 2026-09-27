#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};



struct PointLight {
    vec3 position;
    
    float k_c;  // attenuation factors
    float k_l;  // attenuation factors
    float k_q;  // attenuation factors
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
    float k_c;
    float k_l;
    float k_q;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};



#define NR_POINT_LIGHTS 4
#define NR_SPOT_LIGHTS 2

in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLights[NR_SPOT_LIGHTS];
uniform Material material;

// function prototypes
vec3 CalcPointLight(Material material, PointLight light, vec3 N, vec3 fragPos, vec3 V);
vec3 CalcSpotLight(Material material, SpotLight light, vec3 N, vec3 fragPos, vec3 V);

void main()
{
    // properties
    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);
    
    vec3 result = vec3(0.0);
    // point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(material, pointLights[i], N, FragPos, V);
    for(int i = 0; i < NR_SPOT_LIGHTS; i++)
        result += CalcSpotLight(material, spotLights[i], N, FragPos, V);
      
    FragColor = vec4(result, 1.0);
}

// calculates the color when using a point light.
vec3 CalcPointLight(Material material, PointLight light, vec3 N, vec3 fragPos, vec3 V)
{
    vec3 L = normalize(light.position - fragPos);
    vec3 R = reflect(-L, N);
    
    vec3 K_A = material.ambient;
    vec3 K_D = material.diffuse;
    vec3 K_S = material.specular;
    
    // attenuation
    float d = length(light.position - fragPos);
    float attenuation = 1.0 / (light.k_c + light.k_l * d + light.k_q * (d * d));
    
    vec3 ambient = K_A * light.ambient;
    vec3 diffuse = K_D * max(dot(N, L), 0.0) * light.diffuse;
    vec3 specular = K_S * pow(max(dot(V, R), 0.0), material.shininess) * light.specular;
    
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(Material material, SpotLight light, vec3 N, vec3 fragPos, vec3 V)
{
    vec3 toLight = light.position - fragPos;
    float distance = length(toLight);
    vec3 L = normalize(toLight);
    float theta = dot(normalize(fragPos - light.position), normalize(light.direction));
    float coneIntensity = clamp((theta - light.outerCutOff) /
        (light.cutOff - light.outerCutOff), 0.0, 1.0);
    float attenuation = 1.0 / (light.k_c + light.k_l * distance +
        light.k_q * distance * distance);

    vec3 R = reflect(-L, N);
    vec3 ambient = material.ambient * light.ambient;
    vec3 diffuse = material.diffuse * max(dot(N, L), 0.0) * light.diffuse;
    vec3 specular = material.specular *
        pow(max(dot(V, R), 0.0), material.shininess) * light.specular;

    return ambient * attenuation +
        coneIntensity * attenuation * (diffuse + specular);
}
