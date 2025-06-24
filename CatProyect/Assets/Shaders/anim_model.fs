#version 330 core

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform vec3 ambientLightColor;
uniform float ambientStrength;

#define MAX_POINT_LIGHTS 8

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float radius;
};

uniform int numPointLights;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform vec3 viewPos;

void main()
{
    // Luz ambiental
    vec3 ambient = ambientStrength * ambientLightColor;

    vec3 norm = normalize(Normal);
    vec3 totalDiffuse = vec3(0.0);
    vec3 totalSpecular = vec3(0.0);

    for (int i = 0; i < numPointLights; ++i) {
        vec3 lightDir = normalize(pointLights[i].position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);

        float distance = length(pointLights[i].position - FragPos);
        float attenuation = clamp(1.0 - distance / pointLights[i].radius, 0.0, 1.0);

        // Difusa
        totalDiffuse += diff * pointLights[i].color * pointLights[i].intensity * attenuation;

        // Especular
        float specularStrength = 0.5;
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        totalSpecular += specularStrength * spec * pointLights[i].color * attenuation;
    }

    vec3 textureColor = texture(texture_diffuse1, TexCoords).rgb;
    vec3 result = textureColor * (ambient + totalDiffuse) + totalSpecular;

    FragColor = vec4(result, 1.0);
}
