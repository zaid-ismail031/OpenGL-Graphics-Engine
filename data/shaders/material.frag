#version 330 core

struct Material 
{
    vec3 Ka;       // Ambient color
    vec3 Kd;       // Diffuse color
    vec3 Ks;       // Specular color
    float Ns;      // Specular exponent
    sampler2D map_Ka; // Ambient texture
    sampler2D map_Kd; // Diffuse texture
    sampler2D map_Ks; // Specular texture
};

in vec3 FragPos;   // Fragment position
in vec3 Normal;    // Fragment normal
in vec2 TexCoords; // Texture coordinates

out vec4 FragColor;

uniform Material material;
uniform vec3 lightPos;       // Light position
uniform vec3 viewPos;        // Camera/view position
uniform vec3 lightColor;     // Light color

void main()
{
    // Ambient
    vec3 ambient = material.Ka * texture(material.map_Ka, TexCoords).rgb;

    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = material.Kd * diff * texture(material.map_Kd, TexCoords).rgb;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.Ns);
    vec3 specular = material.Ks * spec * texture(material.map_Ks, TexCoords).rgb;

    vec3 result = lightColor * (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0);
}