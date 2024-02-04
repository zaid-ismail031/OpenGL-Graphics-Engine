#version 330 core

struct Material 
{
    vec3 Ka;       // Ambient color
    vec3 Kd;       // Diffuse color
    vec3 Ks;       // Specular color
    vec3 Ke;       // Emissive color
    float Ns;      // Specular exponent
    sampler2D map_Kd; // Diffuse texture
    sampler2D map_Ks; // Specular texture
    sampler2D map_Bump; // Bump texture
    sampler2D map_Ke; // Emissive texture 
};

in vec3 FragPos;   // Fragment position
in vec3 Normal;    // Fragment normal
in vec2 TexCoord; // Texture coordinates
in vec3 Tangent;   // Tangent vector
in vec3 Bitangent; // Bitangent vector

out vec4 FragColor;

uniform Material material;
uniform vec3 lightPos;       // Light position
uniform vec3 viewPos;        // Camera/view position
uniform vec3 lightColor;     // Light color

void main()
{
    // Transformation from tangent space to world space
    mat3 normalMatrix = mat3(Tangent, Bitangent, Normal);
    
    // Retrieve the normal from the normal map and transform to world space
    vec3 bumpedNormal = texture(material.map_Bump, TexCoord).rgb * 2.0 - 1.0;
    vec3 worldNormal = normalize(normalMatrix * bumpedNormal);

    vec3 lightDirection = normalize(lightPos - FragPos);
    vec3 viewDirection = normalize(viewPos - FragPos);

    // Ambient
    vec3 ambient = material.Ka * vec3(texture(material.map_Kd, TexCoord));
    
    // Diffuse
    float diff = max(dot(worldNormal, lightDirection), 0.0);
    vec3 diffuse = diff * material.Kd * vec3(texture(material.map_Kd, TexCoord));
    
    // Specular
    vec3 reflectDir = reflect(-lightDirection, worldNormal);
    float spec = pow(max(dot(viewDirection, reflectDir), 0.0), material.Ns);
    vec3 specular = spec * material.Ks * vec3(texture(material.map_Ks, TexCoord));
    
    // Emissive
    vec3 emissive = material.Ke * vec3(texture(material.map_Ke, TexCoord));
    
    // Combining all the components
    vec3 result = (ambient + diffuse + specular) * lightColor + emissive;
    FragColor = vec4(result, 1.0);
}