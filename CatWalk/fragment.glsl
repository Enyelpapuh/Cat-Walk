#version 330 core
out vec4 FragColor;

in vec3 Normal;

void main() {
    FragColor = vec4(normalize(Normal) * 0.5 + 0.5, 1.0); // color con normales
}
