#version 330 core

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform vec3 ambientLightColor;  // Color de la luz ambiental
uniform float ambientStrength;   // Intensidad (hazlo alto, como 1.0 o 1.2)

void main()
{
    vec3 ambient = ambientStrength * ambientLightColor;

    vec3 textureColor = texture(texture_diffuse1, TexCoords).rgb;

    vec3 result = textureColor * ambient;

    FragColor = vec4(result, 1.0);
}
