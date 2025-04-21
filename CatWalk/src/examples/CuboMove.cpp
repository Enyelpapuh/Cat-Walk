#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// Variables globales
glm::vec3 cubePosition(0.0f, 0.5f, 0.0f);
bool isJumping = false;
float jumpVelocity = 0.0f;
const float gravity = -9.8f;
const float groundLevel = 0.5f;

// Shaders
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    
    out vec3 vertexColor;  // Salida del color al fragment shader

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main() {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
        vertexColor = aColor;  // Pasar el color al fragment shader
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    in vec3 vertexColor;
    out vec4 FragColor;

    void main() {
        FragColor = vec4(vertexColor, 1.0);  // Usar el color enviado desde el shader de vértices
    }
)";

// Función para compilar los shaders
GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    // Comprobamos si hubo errores durante la compilación
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Error al compilar el shader: " << infoLog << std::endl;
    }
    return shader;
}

// Función para crear el programa de sombreado
GLuint createShaderProgram() {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Comprobamos si hubo errores durante el enlace
    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Error al enlazar el programa de shaders: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

// Datos del cubo (Posiciones y colores)
float vertices[] = {
    // Posiciones          // Colores (Rojo, Verde, Azul)
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  // 0
    0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0.0f,  // 1
    0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 1.0f,  // 2
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,  // 3
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,  // 4
    0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,  // 5
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  // 6
    -0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.5f   // 7
};

// Índices para los triángulos del cubo (cómo se dibujan las caras)
unsigned int indices[] = {
    0, 1, 2,  2, 3, 0,  // Cara frontal
    4, 5, 6,  6, 7, 4,  // Cara trasera
    4, 5, 1,  1, 0, 4,  // Cara inferior
    3, 2, 6,  6, 7, 3,  // Cara superior
    4, 0, 3,  3, 7, 4,  // Cara izquierda
    1, 5, 6,  6, 2, 1   // Cara derecha
};

GLuint VAO, VBO, colorVBO;

// Función para configurar el cubo
void setupCube() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &colorVBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // Posiciones
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // Colores
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

// Función para manejar el teclado
void processInput(GLFWwindow* window, float deltaTime) {
    float speed = 3.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cubePosition.z -= speed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cubePosition.z += speed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cubePosition.x -= speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cubePosition.x += speed;

    // Salto
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !isJumping) {
        isJumping = true;
        jumpVelocity = 5.0f;
    }

    // Aplicar física del salto
    if (isJumping) {
        cubePosition.y += jumpVelocity * deltaTime;
        jumpVelocity += gravity * deltaTime;
        if (cubePosition.y <= groundLevel) {
            cubePosition.y = groundLevel;
            isJumping = false;
        }
    }
}

// Función de renderizado
void render(GLuint shaderProgram) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

    // Matrices de transformación
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, cubePosition);  // Traslación para mover el cubo
    glm::mat4 view = glm::lookAt(glm::vec3(3.0f, 3.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    // Pasar las matrices al shader
    GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// Main
int main() {
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(800, 600, "Cubo OpenGL", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Error cargando GLAD\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    GLuint shaderProgram = createShaderProgram();
    setupCube();

    float lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        processInput(window, deltaTime);
        render(shaderProgram);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
