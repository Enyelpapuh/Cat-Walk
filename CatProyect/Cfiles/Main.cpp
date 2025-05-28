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

//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void processInput(GLFWwindow* window);
void SetCatAnimation(Animation* newAnim);


Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Animation* catAnimation1 = nullptr;
Animation* catAnimation2 = nullptr;
Animation* catAnimation3 = nullptr;
Animation* catAnimation4 = nullptr;
Animation* catAnimation5 = nullptr;

Animator catAnimator1(nullptr);
Animator catAnimator2(nullptr);

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

//skyboxes data
float skyboxVertices[] =
{
	//   Coordinates
	-1.0f, -1.0f,  1.0f,//        7--------6
	 1.0f, -1.0f,  1.0f,//       /|       /|
	 1.0f, -1.0f, -1.0f,//      4--------5 |
	-1.0f, -1.0f, -1.0f,//      | |      | |
	-1.0f,  1.0f,  1.0f,//      | 3------|-2
	 1.0f,  1.0f,  1.0f,//      |/       |/
	 1.0f,  1.0f, -1.0f,//      0--------1
	-1.0f,  1.0f, -1.0f
};

unsigned int skyboxIndices[] =
{
	// Right
	1, 2, 6,
	6, 5, 1,
	// Left
	0, 4, 7,
	7, 3, 0,
	// Top
	4, 5, 6,
	6, 7, 4,
	// Bottom
	0, 3, 2,
	2, 1, 0,
	// Back
	0, 1, 5,
	5, 4, 0,
	// Front
	3, 7, 6,
	6, 2, 3
};


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
	Shader ourShader("Assets/Shaders/anim_model.vs", "Assets/Shaders/anim_model.fs");
	Shader ourShader2("skybox.vert", "skybox.frag");
	ourShader.use();
	ourShader.setVec3("ambientLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	ourShader.setFloat("ambientStrength", 0.9f);
	ourShader2.use();
	ourShader2.setFloat("skyboxIntensity", 0.5f);


	// Create VAO, VBO, and EBO for the skybox
	unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// All the faces of the cubemap (make sure they are in this exact order)
	std::string facesCubemap[6] =
	{
		"Assets/skybox/right.jpg",
		"Assets/skybox/left.jpg",
		"Assets/skybox/top.jpg",
		"Assets/skybox/bottom.jpg",
		"Assets/skybox/front.jpg",
		"Assets/skybox/back.jpg"
	};
	// Creates the cubemap texture object
	unsigned int cubemapTexture;
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// These are very important to prevent seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// This might help with seams on some systems
	//glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// Cycles through all the textures and attaches them to the cubemap object
	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}


	// Configuracion donde se carga el modelo y las animaciones a ocupar de ese modelo
	Model catM1("Assets/Models/Bananin/bananin-danced.fbx", "Assets/Models/Bananin/textures/BananaColor.png");//modelo

	catAnimation1 = new Animation("Assets/Models/Bananin/bananin-danced.fbx", &catM1, "default");//animacion
	catAnimation2 = new Animation("Assets/Models/Bananin/bananin-Run.fbx", &catM1, "run");//animacion
	catAnimation3 = new Animation("Assets/Models/Bananin/bananin-Run.fbx", &catM1, "default");//animacion
	catAnimation4 = new Animation("Assets/Models/Bananin/bananin-Run.fbx", &catM1, "default");//animacion
	catAnimation5 = new Animation("Assets/Models/Bananin/bananin-danced.fbx", &catM1, "default");//animacion


	ourShader.use();
	ourShader.setVec3("ambientLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	ourShader.setFloat("ambientStrength", 0.9f);


	// Carga del sonido
	fmodSystem->createSound("Assets/Angeles.mp3", FMOD_DEFAULT, 0, &startSound); // Reemplaza con tu ruta real
	// Este será el sonido en loop (ej: música de fondo)
	fmodSystem->createSound("Assets/Angeles.mp3", FMOD_LOOP_NORMAL | FMOD_3D, 0, &loopSound);


	if (catAnimation1->HasAnimation()) {
		catAnimator1.AddAnimation(catAnimation1);
		catAnimator1.PlayAnimation(catAnimation1);
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

			if (ImGui::Button("comenzar", ImVec2(200, 50)))
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

			// -------------------- ACTUALIZAR ANIMACIONES --------------------
			catAnimator1.UpdateAnimation(deltaTime);

			// -------------------- SKYBOX --------------------
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = camera.GetViewMatrix();

			glDepthFunc(GL_LEQUAL);  // Permitir z igual
			glDepthMask(GL_FALSE);   // Desactivar escritura de profundidad

			ourShader2.use();
			ourShader2.setMat4("view", glm::mat4(glm::mat3(view))); // sin traslación
			ourShader2.setMat4("projection", projection);

			glBindVertexArray(skyboxVAO);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
			glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			glDepthMask(GL_TRUE);    // Restaurar escritura
			glDepthFunc(GL_LESS);    // Restaurar profundidad estándar

			// -------------------- MODELO ANIMADO --------------------
			ourShader.use();
			ourShader.setMat4("projection", projection);
			ourShader.setMat4("view", view);

			auto cyborgTransforms = catAnimator1.GetFinalBoneMatrices();
			for (int i = 0; i < cyborgTransforms.size(); ++i)
				ourShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", cyborgTransforms[i]);

			glm::mat4 model1 = glm::mat4(1.0f);
			model1 = glm::translate(model1, glm::vec3(-1.0f, -1.0f, 0.0f));
			model1 = glm::scale(model1, glm::vec3(0.01f));
			ourShader.setMat4("model", model1);
			catM1.Draw(ourShader);



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

	delete catAnimation1;
	delete catAnimation2;

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
	{
		camera.ProcessKeyboard(FORWARD, deltaTime);
		SetCatAnimation(catAnimation1);
		std::cout << "cat Animation default" << std::endl;

	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(BACKWARD, deltaTime);
		SetCatAnimation(catAnimation3);
		std::cout << "cat Animation s" << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(LEFT, deltaTime);
		SetCatAnimation(catAnimation4);
		std::cout << "cat Animation a" << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.ProcessKeyboard(RIGHT, deltaTime);
		SetCatAnimation(catAnimation5);
		std::cout << "cat Animation d" << std::endl;
	}
	//if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	//{
	//	camera.ProcessKeyboard(UP, deltaTime);
	//}
	//if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	//{
	//	camera.ProcessKeyboard(DOWN, deltaTime);
	//}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		SetCatAnimation(catAnimation1);
		std::cout << "Cyborg Animation Set" << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		SetCatAnimation(catAnimation2);
		std::cout << "Banana Animation Set" << std::endl;
	}
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
void SetCatAnimation(Animation* newAnim) {
	if (newAnim && newAnim->HasAnimation()) {
		catAnimator1.PlayAnimation(newAnim);
	}
}