#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <UI.h>

#include <FMOD/fmod.hpp>
#include <FMOD/fmod_errors.h> 
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


Camera camera(glm::vec3(0.0f, 0.0f, .0f));
ModelController modelController(glm::vec3(-1.0f, -1.0f, 0.0f));

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

//FMOD
//FMOD_RESULT result;

FMOD::System* fmodSystem;
FMOD::Sound* startSound;
FMOD::Sound* loopSound;       // <- Nuevo sonido que se repetir�
FMOD::Channel* loopChannel;   // <- Canal para controlarlo (opcional)

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

	// Mostrar men� SFML
	Menu menu;
	menu.ejecutar();

	// Si el usuario no presion� "Comenzar", salir
	if (!menu.menuFinalizado()) {
		return 0;
	}

	// Detener la m�sica de SFML antes de iniciar FMOD
	menu.detenerMusica();

	// Inicializaci�n de FMOD
	FMOD_RESULT result;
	result = FMOD::System_Create(&fmodSystem);
	if (result != FMOD_OK) {
		std::cerr << "FMOD error (System_Create): " << FMOD_ErrorString(result) << std::endl;
		return 1;
	}
	result = fmodSystem->init(512, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK) {
		std::cerr << "FMOD error (init): " << FMOD_ErrorString(result) << std::endl;
		return 1;
	}

	result = fmodSystem->createSound("Assets/Music/Musica_Cueva.wav", FMOD_LOOP_NORMAL | FMOD_3D, 0, &loopSound);
	if (result != FMOD_OK) {
		std::cerr << "FMOD error (loopSound): " << FMOD_ErrorString(result) << std::endl;
	}

	// Reproducir la m�sica de FMOD (por ejemplo, la de loop)
	result = fmodSystem->playSound(loopSound, 0, false, &loopChannel);
	if (result != FMOD_OK) {
		std::cerr << "FMOD error (playSound): " << FMOD_ErrorString(result) << std::endl;
	}
	if (loopChannel) {
		loopChannel->setPaused(false);
		loopChannel->setVolume(1.0f);
	}

	GLFWwindow* window = initOpenGL();
	Shader ourShader("Assets/Shaders/anim_model.vs", "Assets/Shaders/anim_model.fs");
	
	// Configuracion donde se carga el modelo y las animaciones a ocupar de ese modelo
	Model catM1("Assets/Models/Bananin/bananin-danced.fbx", "Assets/Models/Bananin/textures/BananaColor.png");//modelo
	Model Mapa("Assets/Models/cueva/cueva.fbx", "Assets/Models/cueva/textures/CaveColor.png");//modelo



	Shader ourShader2("skybox.vert", "skybox.frag");
	ourShader.use();
	ourShader.setVec3("ambientLightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	ourShader.setFloat("ambientStrength", 0.9f);


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
	for(unsigned int i = 0; i < 6; i++)
	{
		// Para la textura right.jpg (i == 0), activar el flip vertical
		if (i == 0)
			stbi_set_flip_vertically_on_load(true);
		else
			stbi_set_flip_vertically_on_load(false);

		int width, height, nrChannels;
		unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
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



	catAnimation1 = new Animation("Assets/Models/Bananin/bananin-danced.fbx", &catM1, "default");//animacion
	catAnimation2 = new Animation("Assets/Models/Bananin/bananin-Run.fbx", &catM1, "run");//animacion
	catAnimation3 = new Animation("Assets/Models/Bananin/bananin-Run.fbx", &catM1, "default");//animacion
	catAnimation4 = new Animation("Assets/Models/Bananin/bananin-Run.fbx", &catM1, "default");//animacion
	catAnimation5 = new Animation("Assets/Models/Bananin/bananin-danced.fbx", &catM1, "default");//animacion


	ourShader.use();
	ourShader.setVec3("ambientLightColor", glm::vec3(1.0f, 1.0f, 1.0f));


	if (catAnimation1->HasAnimation()) {
		catAnimator1.AddAnimation(catAnimation1);
		catAnimator1.PlayAnimation(catAnimation1);
	}

	////
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);

	glBindVertexArray(skyboxVAO);

	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), skyboxIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	////


	while (!glfwWindowShouldClose(window))
	{



		// -------------------- NO TOUCH --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//--------------------- direccion modelo --------------------
		// Calcula la rotación deseada: la dirección contraria a la cámara
		float desiredYaw = camera.Yaw + 180.0f;

		// Normaliza ángulo
		if (desiredYaw > 180.0f)
			desiredYaw -= 360.0f;
		else if (desiredYaw < -180.0f)
			desiredYaw += 360.0f;

		// Suaviza la rotación (evita giros bruscos)
		float rotationSpeed = 5.0f; // Puedes ajustar la velocidad de rotación
		modelController.Yaw = glm::mix(modelController.Yaw, desiredYaw, deltaTime * rotationSpeed);


		//-------------------- PROCESAR ENTRADAS --------------------
		processInput(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// -------------------- ACTUALIZAR ANIMACIONES --------------------
		catAnimator1.UpdateAnimation(deltaTime);



		// Actualizar posición de la cámara para que siga al modelo
		glm::vec3 offset = -modelController.Front * 5.0f + glm::vec3(0.0f, 3.0f, 0.0f);
		camera.Position = modelController.Position + offset;
		camera.Target = modelController.Position;
		// -------------------- SKYBOX --------------------
		// El modelo está en (-1.0, -1.0, 0.0)
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();

		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_FALSE);

		ourShader2.use();
		ourShader2.setMat4("view", glm::mat4(glm::mat3(view))); // sin traslacion
		ourShader2.setMat4("projection", projection);

		glBindVertexArray(skyboxVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawElements(GL_TRIANGLES, sizeof(skyboxIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);


		// -------------------- MODELO ANIMADO --------------------


		ourShader.use();
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);

		ourShader.setBool("isAnimated", true); // <<<< ACTIVAR animación
		if (catAnimator1.HasAnimation())
		{
			auto boneMatrices = catAnimator1.GetFinalBoneMatrices();
			for (int i = 0; i < boneMatrices.size(); ++i)
				ourShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]", boneMatrices[i]);
		}

		//glm::mat4 model1 = modelController.GetModelMatrix();
		//model1 = glm::scale(model1, glm::vec3(0.01f));
		glm::mat4 model1 = glm::mat4(1.0f);
		model1 = glm::translate(model1, modelController.Position);
		model1 = glm::rotate(model1, glm::radians(modelController.Yaw), glm::vec3(0.0f, 1.0f, 0.0f));
		model1 = glm::scale(model1, glm::vec3(0.01f));
		ourShader.setMat4("model", model1);
		catM1.Draw(ourShader);

		// -------------------- MAPA --------------------
		ourShader.setBool("isAnimated", false); // <<<< DESACTIVAR animación
		glm::mat4 model2 = glm::mat4(1.0f);
		model2 = glm::translate(model2, glm::vec3(0.0f, -1.0f, 0.0f));
		model2 = glm::scale(model2, glm::vec3(10.0f));
		model2 = glm::rotate(model2, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		ourShader.setMat4("model", model2);

		Mapa.Draw(ourShader);
		//----------------------------------------------------------------


		// -------------------- FMOD --------------------
		glm::vec3 camPos = camera.Position;
		glm::vec3 camFront = camera.Front;
		glm::vec3 camUp = camera.Up;

		FMOD_VECTOR listenerPos = { camPos.x, camPos.y, camPos.z };
		FMOD_VECTOR listenerVel = { 0.0f, 0.0f, 0.0f };
		FMOD_VECTOR forward = { camFront.x, camFront.y, camFront.z };
		FMOD_VECTOR up = { camUp.x, camUp.y, camUp.z };

		fmodSystem->set3DListenerAttributes(0, &listenerPos, &listenerVel, &forward, &up);

		fmodSystem->update();
		if (loopSound) loopSound->set3DMinMaxDistance(1.0f, 10.0f);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete catAnimation1;
	delete catAnimation2;

	if (startSound) startSound->release();
	if (loopSound) loopSound->release();
	if (fmodSystem) {
		fmodSystem->close();
		fmodSystem->release();
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{

	// --------- Movimiento del modelo ---------
	// Hacer que el modelo apunte donde apunta la cámara
	modelController.SetYaw(camera.Yaw);

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		modelController.ProcessKeyboard(BACKWARD, deltaTime);
		SetCatAnimation(catAnimation1);
		std::cout << "cat Animation default" << std::endl;

	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		modelController.ProcessKeyboard(FORWARD, deltaTime);
		SetCatAnimation(catAnimation3);
		std::cout << "cat Animation s" << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		modelController.ProcessKeyboard(LEFT, deltaTime);
		SetCatAnimation(catAnimation4);
		std::cout << "cat Animation a" << std::endl;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		modelController.ProcessKeyboard(RIGHT, deltaTime);
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