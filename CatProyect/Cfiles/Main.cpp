#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Imgui/imgui.h"
#include "Imgui/imgui_impl_glfw.h"
#include "Imgui/imgui_impl_opengl3.h"

#include <FMOD/fmod.hpp>
#include <chrono>
#include <thread>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "filesystem.h"
#include "shader_m.h"
#include "camera.h"
#include "animator.h"
#include "animation.h"
#include "model_animation.h"

//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>
#include <iostream>

#include "InitGL.h"

extern unsigned int SCR_WIDTH = 800;
extern unsigned int SCR_HEIGHT = 600;

extern float lastX = SCR_WIDTH / 2.0f;
extern float lastY = SCR_HEIGHT / 2.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);



Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Animation* cyborgAnimation = nullptr;
Animation* bananaAnimation = nullptr;

Animator cyborgAnimator(nullptr);
Animator bananaAnimator(nullptr);

//IMGUI
bool runMain = false;
bool startScreen = true;
bool fadeOutStart = false;
float fadeOpacity = 1.0f;

//FMOD
//FMOD_RESULT result;

FMOD::System* fmodSystem;
FMOD::Sound* startSound;
FMOD::Sound* loopSound;       // <- Nuevo sonido que se repetirá
FMOD::Channel* loopChannel;   // <- Canal para controlarlo (opcional)


// Function to render ImGui
void renderImGui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Controls");
	if (ImGui::Button("Run Main"))
	{
		runMain = true;
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int main()
{
	// Sonidos 
	FMOD::System_Create(&fmodSystem);
	fmodSystem->init(512, FMOD_INIT_NORMAL, 0);

	// Cargar un sonido
	fmodSystem->createSound("C:/Users/USUARIO/Downloads/FMOD/FMOD Studio API Windows/api/core/examples/media/swish.wav", FMOD_DEFAULT, 0, &startSound);

	// Este será el sonido en loop (ej: música de fondo)
	fmodSystem->createSound("C:/Users/USUARIO/Downloads/FMOD/FMOD Studio API Windows/api/core/examples/media/singing.wav", FMOD_LOOP_NORMAL | FMOD_3D, 0, &loopSound);


	GLFWwindow* window = initOpenGL();

	Shader ourShader("anim_model.vs", "anim_model.fs");
	ourShader.use();
	ourShader.setVec3("ambientLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	ourShader.setFloat("ambientStrength", 0.9f);

	Model cyborgModel("Assets/Models/cueva/cueva.fbx", "Assets/Models/cueva/textures/caveColor.png");//modelo
	Model bananaModel("Assets/Models/cat2/cat2.fbx", "Assets/Models/cat2/textures/cat2Color.png");//modelo

	cyborgAnimation = new Animation("Assets/Models/cat1/cat1.fbx", &cyborgModel, "sdf|Armature|Armature|EmptyAction");//animacion
	bananaAnimation = new Animation("Assets/Models/cat2/cat2.fbx", &bananaModel, "dfsdfsd");//animacion

	if (cyborgAnimation->HasAnimation()) {
		cyborgAnimator.AddAnimation(cyborgAnimation);
		cyborgAnimator.PlayAnimation(cyborgAnimation);
	}
	if (bananaAnimation->HasAnimation()) {
		bananaAnimator.AddAnimation(bananaAnimation);
		bananaAnimator.PlayAnimation(bananaAnimation);
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	GLuint backgroundTexture = 0; // Inicializa en 0 (valor inválido)
	int imageWidth = 0, imageHeight = 0, nrChannels = 0;
	unsigned char* imageData = stbi_load("Assets/Models/Bananin/textures/BananaColor.png", &imageWidth, &imageHeight, &nrChannels, 0);

	if (imageData)
	{
		glGenTextures(1, &backgroundTexture);
		glBindTexture(GL_TEXTURE_2D, backgroundTexture);

		// Determina el formato adecuado según los canales
		GLenum format = GL_RGB;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)//jpg rgb
			format = GL_RGB;
		else if (nrChannels == 4)//png rgba
			format = GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, imageWidth, imageHeight, 0, format, GL_UNSIGNED_BYTE, imageData);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		stbi_image_free(imageData);
	}
	else
	{
		std::cerr << "No se pudo cargar la imagen de fondo:" << std::endl;
		backgroundTexture = 0; // Asegura que el valor sea inválido
	}

	// Antes de usar la textura, verifica que sea válida
	if (backgroundTexture == 0)
	{
		// Puedes mostrar un mensaje, usar una textura por defecto, o evitar dibujar la imagen
		std::cerr << "Error: La textura de fondo no está inicializada correctamente." << std::endl;
		// return o manejo alternativo según tu lógica
	}




	// Carga del sonido
	fmodSystem->createSound("Assets/Angeles.mp3", FMOD_DEFAULT, 0, &startSound); // Reemplaza con tu ruta real
	// Este será el sonido en loop (ej: música de fondo)
	fmodSystem->createSound("Assets/Angeles.mp3", FMOD_LOOP_NORMAL | FMOD_3D, 0, &loopSound);


	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwSetInputMode(window, GLFW_CURSOR, startScreen ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (startScreen)
		{
			ImTextureID my_tex_id = (ImTextureID)(intptr_t)backgroundTexture;


			ImGuiIO& io = ImGui::GetIO();

			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(io.DisplaySize);

			ImGui::Begin("PantallaPrincipal", nullptr,
				ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove |
				ImGuiWindowFlags_NoScrollbar |
				ImGuiWindowFlags_NoCollapse
			);

			// Imagen de fondo
			ImGui::GetWindowDrawList()->AddImage(
				(ImTextureID)(intptr_t)backgroundTexture,
				ImGui::GetWindowPos(),
				ImVec2(ImGui::GetWindowPos().x + io.DisplaySize.x,
					ImGui::GetWindowPos().y + io.DisplaySize.y)
			);

			// Posicionar botón al centro
			ImGui::SetCursorPos(ImVec2(io.DisplaySize.x / 2 - 50, io.DisplaySize.y / 2 - 15));

			// Estilo: botón oscuro con texto blanco
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));       // Fondo botón oscuro
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f)); // Al pasar el mouse
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));  // Al presionar
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));                      // Texto blanco


			// Espacio para que el botón no se superponga con los bordes
			ImGui::SetCursorPos(ImVec2(io.DisplaySize.x / 2 - 50, io.DisplaySize.y / 2 - 15));

			if (ImGui::Button("Pruebadano", ImVec2(200, 50)))
			{
				fmodSystem->playSound(startSound, 0, false, 0);  // <--- Aquí se reproduce el sonido
				fadeOutStart = true;

				std::this_thread::sleep_for(std::chrono::milliseconds(2000));

				FMOD_VECTOR soundPos = { 0.0f, 0.0f, 0.0f };  // Posición donde se cargan los modelos
				FMOD_VECTOR velocity = { 0.0f, 0.0f, 0.0f };  // Velocidad (puede ser 0 si está estático)

				// Después de reproducir el sonido:
				fmodSystem->playSound(loopSound, 0, false, &loopChannel);
				loopChannel->set3DAttributes(&soundPos, &velocity);

			}

			// Restaurar estilos
			ImGui::PopStyleColor(4);

			ImGui::End();

			if (fadeOutStart)
			{
				fadeOpacity -= deltaTime;
				if (fadeOpacity <= 0.0f)
				{
					fadeOpacity = 0.0f;
					startScreen = false;
				}
			}

			ImGui::GetStyle().Alpha = fadeOpacity;
		}
		else
		{
			processInput(window);

			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			cyborgAnimator.UpdateAnimation(deltaTime);
			bananaAnimator.UpdateAnimation(deltaTime);

			ourShader.use();

			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = camera.GetViewMatrix();
			ourShader.setMat4("projection", projection);
			ourShader.setMat4("view", view);

			auto cyborgTransforms = cyborgAnimator.GetFinalBoneMatrices();
			for (int i = 0; i < cyborgTransforms.size(); ++i)
				ourShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", cyborgTransforms[i]);

			glm::mat4 model1 = glm::mat4(1.0f);
			model1 = glm::translate(model1, glm::vec3(-1.0f, -1.0f, 0.0f));
			model1 = glm::scale(model1, glm::vec3(0.01f));
			ourShader.setMat4("model", model1);
			cyborgModel.Draw(ourShader);

			auto bananaTransforms = bananaAnimator.GetFinalBoneMatrices();
			for (int i = 0; i < bananaTransforms.size(); ++i)
				ourShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", bananaTransforms[i]);

			glm::mat4 model2 = glm::mat4(1.0f);
			model2 = glm::translate(model2, glm::vec3(1.0f, -1.0f, 0.0f));
			model2 = glm::scale(model2, glm::vec3(0.01f));
			ourShader.setMat4("model", model2);
			bananaModel.Draw(ourShader);

			glm::vec3 camPos = camera.Position;
			glm::vec3 camFront = camera.Front;
			glm::vec3 camUp = camera.Up;

			FMOD_VECTOR listenerPos = { camPos.x, camPos.y, camPos.z };
			FMOD_VECTOR listenerVel = { 0.0f, 0.0f, 0.0f };  // O calcula el movimiento si quieres efectos doppler
			FMOD_VECTOR forward = { camFront.x, camFront.y, camFront.z };
			FMOD_VECTOR up = { camUp.x, camUp.y, camUp.z };

			fmodSystem->set3DListenerAttributes(0, &listenerPos, &listenerVel, &forward, &up);

		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		fmodSystem->update();

		loopSound->set3DMinMaxDistance(1.0f, 10.0f);  // minDist, maxDist

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete cyborgAnimation;
	delete bananaAnimation;

	startSound->release();
	fmodSystem->close();
	fmodSystem->release();
	loopSound->release();

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}