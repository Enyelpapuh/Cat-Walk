#version 330 core
out vec4 FragColor;

in vec3 texCoords;

uniform samplerCube skybox;
uniform float nightFactor; // 0.0 = sin oscurecer, 1.0 = máximo oscurecido

void main()
{
    vec3 color = texture(skybox, texCoords).rgb;
    // Oscurece el color: 0.2 es el mínimo (más oscuro), ajusta a tu gusto
    color *= mix(1.0, 0.2, nightFactor);
    FragColor = vec4(color, 1.0);
}
