#version 330 core
out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

struct Material{
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;

    float shininess;
};

struct DirectionalLight{
    bool enabled;
    vec3 direction;
    vec3 color;

    // Attribute Strength
    float specular; 
    float diffuse;
    float ambient;
};

struct PointLight{
    bool enabled;
    vec3 position;
    vec3 color;
    
    // Attribute Strength
    float specular;
    float diffuse;
    float ambient;

    // Attenuation
    float constant, linear, quadratic;
};

struct SpotLight{
    bool enabled;
    vec3 position;
    vec3 direction;
    vec3 color;
    
    // Attribute Strength
    float specular;
    float diffuse;
    float ambient;

    // SpotLight Variables
    float cutoff;

    // Attenuation
    float constant, linear, quadratic;
};

uniform Material material;
uniform DirectionalLight dirLight;
uniform PointLight pntLight;
uniform SpotLight SpotLight1;

uniform vec3 viewPos;

vec3 CalcSpotLight(SpotLight sptLight, vec3 normal, vec3 viewDir, vec2 texCoords){
    vec3 lightDir = normalize(sptLight.position - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float theta = dot(lightDir, normalize(-sptLight.direction)); // Radius angle

    if(theta > sptLight.cutoff && sptLight.enabled){
        float dist = length(sptLight.position - FragPos);
        float attenuation = 1 / (sptLight.constant + (sptLight.linear * dist) + (sptLight.quadratic * dist * dist));

        // ambient
        vec3 ambient = sptLight.color * sptLight.ambient * texture(material.texture_diffuse1, texCoords).rgb;

        // Diffuse
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = sptLight.diffuse * diff * sptLight.color * texture(material.texture_diffuse1, texCoords).rgb;

        // Specular
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = sptLight.specular * sptLight.color * spec * texture(material.texture_specular1, texCoords).rgb;

        return (ambient + diffuse + specular) * attenuation;

    }else{
        vec3 ambient = sptLight.color * sptLight.ambient * texture(material.texture_diffuse1, texCoords).rgb;
        return ambient;
    }
}

vec3 CalcDirLight(DirectionalLight DirLight, vec3 normal, vec3 viewDir, vec2 texCoords){
    vec3 lightDir = normalize(-DirLight.direction);
    vec3 reflectDir = reflect(-lightDir, normal);

    // Ambient
    vec3 ambient = DirLight.color * DirLight.ambient * texture(material.texture_diffuse1, texCoords).rgb;

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = DirLight.color * DirLight.diffuse * diff * texture(material.texture_diffuse1, texCoords).rgb;

    // Specular
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = DirLight.color * DirLight.specular * spec * texture(material.texture_specular1, texCoords).rgb;
    if(dirLight.enabled)
        return ambient + diffuse + specular;
    else
        return ambient;
}

vec3 CalcPointLight(PointLight pointLight, vec3 normal, vec3 viewDir, vec2 texCoords){
    vec3 lightDir = normalize(pointLight.position - FragPos);
    float dist = length(pointLight.position - FragPos);
    float attenuation = 1 / (pointLight.constant + (pointLight.linear * dist) + (pointLight.quadratic * dist * dist));
    vec3 reflectDir = reflect(-lightDir, normal);

    // ambient
    vec3 ambient = pointLight.color * pointLight.ambient * texture(material.texture_diffuse1, texCoords).rgb;

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = pointLight.color * pointLight.diffuse * diff * texture(material.texture_diffuse1, texCoords).rgb;

    // Specular
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = pointLight.color * spec * pointLight.specular * texture(material.texture_specular1, texCoords).rgb;

    if(pointLight.enabled)
        return (ambient + specular + diffuse) * attenuation;
    else
        return ambient;
}

void main()
{    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);

    result += CalcDirLight(dirLight, norm, viewDir, TexCoords);
    result += CalcPointLight(pntLight, norm, viewDir, TexCoords);
    result += CalcSpotLight(SpotLight1, norm, viewDir, TexCoords);

    FragColor = vec4(result, 1.0);
}