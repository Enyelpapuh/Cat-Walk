#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <UI.h>

#include <FMOD/fmod.hpp>
#include <FMOD/fmod_errors.h> 
#include <chrono>
#include <thread>
#include <atomic>
#include <SFML/Graphics.hpp>

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
// Bullet
#include "PhysicsManager.h"

extern unsigned int SCR_WIDTH = 1920;
extern unsigned int SCR_HEIGHT = 1080;

extern float lastX = SCR_WIDTH / 2.0f;
extern float lastY = SCR_HEIGHT / 2.0f;

// Esta función lanza el menú de pausa en un hilo aparte

std::atomic<bool> pausaActiva(false);
std::atomic<bool> juegoPausado(false);

Camera camera(glm::vec3(0.0f, 0.0f, 2.0f));
ModelController modelController(glm::vec3(0.0f, 1.0f, 0.0f));

bool firstMouse = true;
bool isMovingForward = false;
bool saltoIniciado = false;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Animation* catAnimation1 = nullptr;
Animation* catAnimation2 = nullptr;
Animation* catAnimation3 = nullptr;
Animation* catAnimation4 = nullptr;
Animation* catAnimation5 = nullptr;

// -------------------- Estado de animación --------------------
enum class AnimState { Idle, WalkFwd, WalkBack, StrafeL, StrafeR };

void SetCatAnimation(Animation* newClip, AnimState newState);

AnimState gCurrentState = AnimState::Idle;   // lo que YA se reproduce
Animation* gCurrentClip = nullptr;

Animator catAnimator1(nullptr);
//Physics init
PhysicsManager physics;
btRigidBody* catRigidBody = physics.CreateDynamicBox(glm::vec3(0, 1.0, 0), glm::vec3(0.4f, 0.6f, 0.4f), 1.0f);

//FMOD
//FMOD_RESULT result;

FMOD::System* fmodSystem;
FMOD::Sound* startSound;
FMOD::Sound* loopSound;       // <- Nuevo sonido que se repetir�
FMOD::Channel* loopChannel;   // <- Canal para controlarlo (opcional)

void lanzarMenuPausa(Menu* menu) {
	pausaActiva = true;
	juegoPausado = true; // Pausa el juego

	// Pausar la música de FMOD si está sonando
	if (loopChannel) {
		loopChannel->setPaused(true);
	}

	bool continuar = menu->ejecutarPausa();

	// Reanudar la música de FMOD si corresponde
	if (loopChannel) {
		loopChannel->setPaused(false);
	}

	pausaActiva = false;
	juegoPausado = false; // Reanuda el juego

	if (!continuar) {
		std::exit(0); // Cierra todo el programa si se presionó "Salir"
	}
}

//skyboxes data
float skyboxVertices[] =
{
	//   Coordinates
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
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
	bool enElSuelo = false; // Local, no global
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

	GLFWwindow* window = nullptr;
	if (menu.esPantallaCompleta()) {
		window = initOpenGL(true); // true = fullscreen
	}
	else {
		window = initOpenGL(false); // false = ventana normal
	}

	//-------------crear el cubo de la hitbox----------------------------
	GLuint cubeVAO, cubeVBO;
	float cubeVertices[] = {
		// 8 vértices del cubo
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f
	};

	unsigned int cubeIndices[] = {
		0, 1, 1, 2, 2, 3, 3, 0, // atrás
		4, 5, 5, 6, 6, 7, 7, 4, // adelante
		0, 4, 1, 5, 2, 6, 3, 7  // conexiones entre cara frontal y trasera
	};

	GLuint cubeEBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glGenBuffers(1, &cubeEBO);

	glBindVertexArray(cubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindVertexArray(0);
	//-------------------------------------------------------------------


	// Después de inicializar OpenGL y antes del loop principal:
	GLuint dbgVAO = 0, dbgVBO = 0;
	glGenVertexArrays(1, &dbgVAO);
	glGenBuffers(1, &dbgVBO);
	Shader ourShader("Assets/Shaders/anim_model.vs", "Assets/Shaders/anim_model.fs");
	Shader lineShader("Assets/Shaders/debug_line.vs", "Assets/Shaders/debug_line.fs");

	// Configuracion donde se carga el modelo y las animaciones a ocupar de ese modelo
	Model catM1("Assets/Models/GATO/Gato_Default.fbx", "Assets/Models/GATO/textures/CatColor.png");//modelo
	Model faroModel("Assets/Models/Faro/Faro.fbx", "Assets/Models/Faro/textures/FaroColor.png");
	glm::vec3 faroPosition = glm::vec3(10.0f, 0.0f, 5.0f); // Cambia la posición según lo necesites

	Model Mapa("Assets/Models/cueva/city2.fbx", "Assets/Models/cueva/textures/ParkColor1.png");//modelo
	// Sincronizar la dirección inicial del modelo con la cámara
	modelController.Yaw = camera.Yaw;
	modelController.updateVectors();
	// Crear colisionador dinámico para el gato
	btRigidBody* catRigidBody = physics.CreateDynamicBox(
		glm::vec3(0, 1.0f, 0),      // posición inicial
		glm::vec3(0.5f, 0.5f, 1.0f), // tamaño caja (ancho, alto, profundidad)
		1.0f                      // masa
	);

	// Configurar el shader del gato
	lineShader.use();
	// Define the projection matrix before using it
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();

	// Use the projection matrix in the shaderdddd
	lineShader.setMat4("projection", projection);
	lineShader.setMat4("view", view);
	// Crear colisionador estático del mapa
// Escala NO uniforme (correcto si quieres 20x4x20)
// Obtener la AABB del modelo
	glm::vec3 min = Mapa.GetAABBMin();
	glm::vec3 max = Mapa.GetAABBMax();

	// Validar la AABB antes de crear el colisionador
	if (min.x == FLT_MAX || min.y == FLT_MAX || min.z == FLT_MAX ||
		max.x == -FLT_MAX || max.y == -FLT_MAX || max.z == -FLT_MAX) {
		std::cerr << "Error: La AABB del modelo de mapa no es válida. No se creará el colisionador." << std::endl;
	}
	else {
		btVector3 btMin = glmToBt(min);
		btVector3 btMax = glmToBt(max);

		// Ahora puedes crear el colisionador usando btMin y btMax, o pasar el modelo a tu PhysicsManager
		btRigidBody* mapRigidBody = physics.CreateStaticMeshFromModel(
			Mapa,
			glm::vec3(4.0f, 4.0f, 4.0f), // Escala
			glm::vec3(-90.0f, 0.0f, 0.0f) // Rotación
		);
	}



	Shader ourShader2("Assets/Shaders/skybox.vert", "Assets/Shaders/skybox.frag");

	ourShader.use();
	ourShader.setVec3("ambientLightColor", glm::vec3(0.2f, 0.2f, 0.3f)); // Azul tenue
	ourShader.setFloat("ambientStrength", 1.0f);
	// Configura la luz puntual del faro
	glm::vec3 faroPos = glm::vec3(10.0f, 3.0f, 5.0f); // Ejemplo: posición del faro
	ourShader.setVec3("pointLightPos", faroPos);
	ourShader.setVec3("pointLightColor", glm::vec3(1.0f, 0.95f, 0.8f)); // Luz cálida
	ourShader.setFloat("pointLightIntensity", 3.0f); // Más fuerte para compensar la noche
	ourShader.setFloat("pointLightRadius", 20.0f);   // Ajusta el área iluminada
	ourShader.setVec3("viewPos", camera.Position);



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


	catAnimation1 = new Animation("Assets/Models/GATO/Gato_Default2.fbx", &catM1, "default");//animacion
	catAnimation2 = new Animation("Assets/Models/GATO/Gato_Walk.fbx", &catM1, "run");//animacion
	catAnimation3 = new Animation("Assets/Models/GATO/Gato.fbx", &catM1, "running");//animacion
	catAnimation4 = new Animation("Assets/Models/GATO/Gato_Default.fbx", &catM1, "default");//animacion
	catAnimation5 = new Animation("Assets/Models/GATO/Gato_Default.fbx", &catM1, "default");//animacion




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

	bool puedePausar = true;

	while (!glfwWindowShouldClose(window))
	{



		// -------------------- TIEMPO --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//-------------------- PROCESAR ENTRADAS --------------------
		processInput(window, catRigidBody, deltaTime, enElSuelo);

		// -------------------- FÍSICA --------------------
		physics.StepSimulation(deltaTime);
		physics.DebugDrawWorld(); // <── NUEVO: dibujar el mundo de Bullet

		//---------Obtener posición actual del cuerpo del gato (controlado por física)
		btTransform transform;
		catRigidBody->getMotionState()->getWorldTransform(transform);
		btVector3 pos = transform.getOrigin();
		modelController.Position = glm::vec3(pos.x(), pos.y(), pos.z());

		// Raycast desde la base del collider hacia abajo
		glm::vec3 groundNormal(0.0f, 0.5f, 0.0f); // Normal por defecto
		float halfHeight = 0.3f; // O el valor real de la mitad de tu collider
		btVector3 from = pos - btVector3(0, halfHeight, 0);
		btVector3 to = from - btVector3(0, 0.5f, 0);
		btCollisionWorld::ClosestRayResultCallback rayCallback(from, to);
		physics.GetDynamicsWorld()->rayTest(from, to, rayCallback);

		bool estabaEnElSuelo = enElSuelo; // Guarda el estado anterior

		enElSuelo = false;
		if (rayCallback.hasHit()) {
			float d = from.getY() - rayCallback.m_hitPointWorld.getY();
			// Puedes ajustar el umbral según el tamaño del collider
			if (d >= -0.5f && d < 0.5f) { // umbral típico para contacto
				enElSuelo = true;
				groundNormal = glm::vec3(rayCallback.m_hitNormalWorld.x(), rayCallback.m_hitNormalWorld.y(), rayCallback.m_hitNormalWorld.z());
				std::cout << "Distancia base al suelo: " << d << " | Normal Y: " << rayCallback.m_hitNormalWorld.y() << std::endl;
			}
		}



		// Imprime solo si hay cambio de estado
		if (enElSuelo && !estabaEnElSuelo) {
			std::cout << "¡Tocó el suelo!" << std::endl;
		}
		if (!enElSuelo && estabaEnElSuelo) {
			std::cout << "¡En el aire!" << std::endl;
		}

		if (rayCallback.hasHit()) {
			btVector3 n = rayCallback.m_hitNormalWorld;
			groundNormal = glm::vec3(n.x(), n.y(), n.z());
			enElSuelo = true; // Si el raycast golpea algo, asumimos que está en el suelo
		}


		// 3. Usa groundNormal para la inclinación del modelo (tu bloque original)
		glm::vec3 up = glm::normalize(groundNormal);
		glm::vec3 baseFront = glm::vec3(0.0f, 0.0f, 1.0f);
		glm::mat4 yawMat = glm::rotate(glm::mat4(1.0f), glm::radians(modelController.Yaw - 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec3 front = glm::normalize(glm::vec3(yawMat * glm::vec4(baseFront, 0.0f)));
		glm::vec3 right = glm::normalize(glm::cross(up, front));
		front = glm::normalize(glm::cross(right, up));
		glm::mat4 rotMat = glm::mat4(1.0f);
		rotMat[0] = glm::vec4(right, 0.0f);
		rotMat[1] = glm::vec4(up, 0.0f);
		rotMat[2] = glm::vec4(-front, 0.0f);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//--------------------bullets physics-------------------
		static float modelPitch = 0.0f;
		float targetPitch = 0.0f;
		float velY = catRigidBody->getLinearVelocity().y();

		if (!enElSuelo) {
			if (velY > 0.1f) {
				// Está subiendo (salto)
				targetPitch = 25.0f; // Ajusta el ángulo a tu gusto
			}
			else if (velY < -0.1f) {
				// Está cayendo
				targetPitch = -45.0f;
			}
			else {
				// En el aire pero casi sin velocidad vertical
				targetPitch = 0.0f;
			}
		}
		else {
			// En el suelo
			targetPitch = 0.0f;
			saltoIniciado = false;
		}

		float interpSpeed = 5.0f;
		modelPitch += (targetPitch - modelPitch) * deltaTime * interpSpeed;


		// --- PAUSE MENU LOGIC BEGIN HERE ---
		static bool puedePausar = true;
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && puedePausar && !pausaActiva) {
			puedePausar = false;
			std::thread pausaThread(lanzarMenuPausa, &menu);
			pausaThread.detach();
		}
		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE) {
			puedePausar = true;
		}

		// --- PAUSE GAME LOOP ---
		if (juegoPausado) {
			// Opcional: puedes poner aquí lógica para mostrar un overlay de "Pausado" si quieres
			glfwPollEvents(); // Permite cerrar la ventana principal con la X
			continue; // Salta el resto del bucle hasta que se reanude
		}

		// --- PAUSE MENU LOGIC FINISH HERE ---
		//--------------------- direccion modelo --------------------
	
		// Calcula la rotación deseada: la dirección contraria a la cámara
		//float desiredYaw = camera.Yaw + 180.0f;

		//// Normaliza ángulo
		//if (desiredYaw > 180.0f)
		//	desiredYaw -= 360.0f;
		//else if (desiredYaw < -180.0f)
		//	desiredYaw += 360.0f;

		//// Suaviza la rotación (evita giros bruscos)
		//float rotationSpeed = 5.0f; // Puedes ajustar la velocidad de rotación
		//modelController.Yaw = glm::mix(modelController.Yaw, desiredYaw, deltaTime * rotationSpeed);


		// -------------------- ACTUALIZAR ANIMACIONES --------------------
		catAnimator1.UpdateAnimation(deltaTime);



		// Actualizar posición de la cámara para que siga al modelo
		glm::vec3 offset = -modelController.Front * 5.0f + glm::vec3(0.0f, 10.0f, 0.0f);
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
		ourShader2.setFloat("nightFactor", 1.0f);

		glBindVertexArray(skyboxVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawElements(GL_TRIANGLES, sizeof(skyboxIndices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);



		// -------------------- MODELO ANIMADO --------------------
		//// --- Dibuja el rayo del raycast (láser) ---
		//glm::vec3 rayStart(from.x(), from.y(), from.z());
		//glm::vec3 rayEnd(to.x(), to.y(), to.z());
		//glm::vec3 rayVertices[2] = { rayStart, rayEnd };

		//GLuint rayVBO, rayVAO;
		//glGenVertexArrays(1, &rayVAO);
		//glGenBuffers(1, &rayVBO);

		//glBindVertexArray(rayVAO);
		//glBindBuffer(GL_ARRAY_BUFFER, rayVBO);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(rayVertices), rayVertices, GL_DYNAMIC_DRAW);
		//glEnableVertexAttribArray(0);
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

		//glm::vec3 color = enElSuelo ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0); // Verde si toca suelo, rojo si no
		//lineShader.use();
		//lineShader.setMat4("projection", projection);
		//lineShader.setMat4("view", view);
		//lineShader.setMat4("model", glm::mat4(1.0f));
		//lineShader.setVec3("lineColor", color);

		//glBindVertexArray(rayVAO);
		//glDrawArrays(GL_LINES, 0, 2);
		//glBindVertexArray(0);

		//// Limpieza
		//glDeleteBuffers(1, &rayVBO);
		//glDeleteVertexArrays(1, &rayVAO);

		/* ───────── matriz WORLD que viene del rigid‑body ───────── */
		btTransform t;
		catRigidBody->getMotionState()->getWorldTransform(t);      // pos. actual del cuerpo

		glm::mat4 world(1.0f);
		world = glm::translate(world,
			glm::vec3(t.getOrigin().getX(),
				t.getOrigin().getY(),
				t.getOrigin().getZ()));



		/* ────────── 1) modelo animado (escala 0.01 del FBX) ────────── */
		float offsetY = -0.4f;
		glm::mat4 modelMat = glm::mat4(1.0f);
		modelMat = glm::translate(modelMat, modelController.Position + glm::vec3(0.0f, offsetY, 0.0f));
		modelMat *= rotMat;
		modelMat = glm::rotate(modelMat, glm::radians(modelPitch), glm::vec3(0.0f, 0.0f, 1.0f)); // Aplica la inclinación
		modelMat = glm::scale(modelMat, glm::vec3(0.01f));





		ourShader.use();
		ourShader.setMat4("projection", projection);
		ourShader.setMat4("view", view);
		ourShader.setMat4("model", modelMat);
		ourShader.setBool("isAnimated", true);

		if (catAnimator1.HasAnimation())
		{
			auto boneMats = catAnimator1.GetFinalBoneMatrices();
			for (int i = 0; i < boneMats.size(); ++i)
				ourShader.setMat4("finalBonesMatrices[" + std::to_string(i) + "]",
					boneMats[i]);
		}
		catM1.Draw(ourShader);

		///* ────────── 2) cubo hitbox (azul) ────────── */
		//// Usa los mismos half extents que el collider de Bullet
		//glm::vec3 halfExt(0.5f, 0.5f, 1.0f);
		//// Aplica la rotación y el escalado al cubo
		//glm::mat4 cubeMat = world
		//	* glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.4f, 0.0f))
		//	* rotMat
		//	* glm::rotate(glm::mat4(1.0f), glm::radians(modelPitch), glm::vec3(0.0f, 0.0f, 1.0f))
		//	* glm::scale(glm::mat4(1.0f), halfExt);
		//// Configura el shader para las líneas de debug

		//lineShader.use();
		//lineShader.setMat4("projection", projection);
		//lineShader.setMat4("view", view);
		//lineShader.setMat4("model", cubeMat);
		//lineShader.setVec3("lineColor", glm::vec3(0, 1, 0));

		//glBindVertexArray(cubeVAO);
		//glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0);

		///* ────────── 3) líneas de debug del mundo Bullet (verde) ────────── */
		//const auto& lines = physics.GetDebugDrawer()->lines;
		//if (!lines.empty())
		//{
		//	glBindVertexArray(dbgVAO);
		//	glBindBuffer(GL_ARRAY_BUFFER, dbgVBO);
		//	glBufferData(GL_ARRAY_BUFFER,
		//		lines.size() * sizeof(glm::vec3),
		//		lines.data(), GL_DYNAMIC_DRAW);

		//	lineShader.use();
		//	lineShader.setMat4("projection", projection);
		//	lineShader.setMat4("view", view);
		//	lineShader.setMat4("model", glm::mat4(1.0f));


		//	glDisable(GL_DEPTH_TEST);
		//	glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(lines.size()));
		//	glEnable(GL_DEPTH_TEST);

		//	glBindVertexArray(0);
		//	physics.GetDebugDrawer()->Clear();
		//}
		// -------------------- FARO --------------------

		std::vector<glm::vec3> posicionesFaros = {
			// Parte trasera (z negativo)
			glm::vec3(-8.0f, -6.0f, -48.0f), // izquierda abajo
			glm::vec3(7.0f, -6.0f, -48.0f),  // derecha abajo

			// Parte derecha (x positivo)
			glm::vec3(46.0f, -6.0f, -7.0f),  // derecha abajo
			glm::vec3(46.0f, -5.0f, 8.0f),   // derecha arriba

			// Parte frontal (z positivo)
			glm::vec3(-8.0f, -6.0f, 48.0f),  // izquierda abajo
			glm::vec3(7.0f, -6.0f, 48.0f),   // derecha abajo

			// Parte izquierda (x negativo)
			glm::vec3(-48.0f, -6.0f, -7.0f), // izquierda abajo
			glm::vec3(-48.0f, -5.0f, 8.0f)   // izquierda arriba
		};

		ourShader.use();
		ourShader.setInt("numPointLights", posicionesFaros.size());
		for (size_t i = 0; i < posicionesFaros.size(); ++i) {
			ourShader.setVec3("pointLights[" + std::to_string(i) + "].position", posicionesFaros[i]);
			ourShader.setVec3("pointLights[" + std::to_string(i) + "].color", glm::vec3(1.0f, 0.95f, 0.8f)); // o azul si quieres
			ourShader.setFloat("pointLights[" + std::to_string(i) + "].intensity", 3.5f);
			ourShader.setFloat("pointLights[" + std::to_string(i) + "].radius", 25.0f);
		}
		ourShader.setVec3("viewPos", camera.Position);
		for (const auto& pos : posicionesFaros) {
			glm::mat4 faroModelMat = glm::mat4(1.0f);
			faroModelMat = glm::translate(faroModelMat, pos);
			faroModelMat = glm::scale(faroModelMat, glm::vec3(2.0f, 1.0f, 2.0f));
			faroModelMat = glm::rotate(faroModelMat, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

			ourShader.use();
			ourShader.setMat4("model", faroModelMat);
			ourShader.setBool("isAnimated", false);
			faroModel.Draw(ourShader);
		}


		// -------------------- MAPA --------------------
		ourShader.use();
		ourShader.setBool("isAnimated", false); // <<<< DESACTIVAR animación
		glm::mat4 model2 = glm::mat4(1.0f);
		model2 = glm::translate(model2, glm::vec3(0.0f, 0.0f, 0.0f));
		model2 = glm::scale(model2, glm::vec3(4.0f, 4.0f, 4.0f));
		model2 = glm::rotate(model2, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		ourShader.setMat4("model", model2);

		Mapa.Draw(ourShader);
		//----------------------------------------------------------------


		//// -------------------- FMOD --------------------
		//glm::vec3 camPos = camera.Position;
		//glm::vec3 camFront = camera.Front;
		//glm::vec3 camUp = camera.Up;

		//FMOD_VECTOR listenerPos = { camPos.x, camPos.y, camPos.z };
		//FMOD_VECTOR listenerVel = { 0.0f, 0.0f, 0.0f };
		//FMOD_VECTOR forward = { camFront.x, camFront.y, camFront.z };
		//FMOD_VECTOR up = { camUp.x, camUp.y, camUp.z };

		//fmodSystem->set3DListenerAttributes(0, &listenerPos, &listenerVel, &forward, &up);

		//fmodSystem->update();
		//if (loopSound) loopSound->set3DMinMaxDistance(1.0f, 10.0f);

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

void processInput(GLFWwindow* window,
	btRigidBody* body,
	float deltaTime, bool& enElSuelo)
{
	bool isMoving = false;
	bool isRunning = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);

	glm::vec3 forward = glm::normalize(glm::vec3(camera.Front.x, 0.0f, camera.Front.z));
	glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0, 1, 0)));

	glm::vec3 v(0.0f);

	// Cambia la velocidad según si está corriendo o no
	if (isRunning) {
		modelController.MovementSpeed = 8.0f;
	}
	else {
		modelController.MovementSpeed = 4.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && enElSuelo) {
		body->setLinearVelocity(btVector3(
			body->getLinearVelocity().x(),
			7.0f,
			body->getLinearVelocity().z()));
		enElSuelo = false;
		saltoIniciado = true;
		std::cout << "Salto aplicado, velocidad Y: " << body->getLinearVelocity().y() << std::endl;
	}

	// Movimiento basado en teclas
	Animation* moveAnim = isRunning ? catAnimation3 : catAnimation2; // run o walk

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		v += forward;
		SetCatAnimation(moveAnim, AnimState::WalkFwd);
		isMoving = true;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		v -= forward;
		SetCatAnimation(moveAnim, AnimState::WalkBack);
		isMoving = true;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		v -= right;
		SetCatAnimation(moveAnim, AnimState::StrafeL);
		isMoving = true;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		v += right;
		SetCatAnimation(moveAnim, AnimState::StrafeR);
		isMoving = true;
	}

	if (!isMoving) {
		SetCatAnimation(catAnimation1, AnimState::Idle);
		isMovingForward = false;
	}

	if (glm::length(v) > 0.01f) {
		v = glm::normalize(v) * modelController.MovementSpeed;
		btVector3 cur = body->getLinearVelocity();
		body->setLinearVelocity(btVector3(v.x, cur.getY(), v.z));

		float newYaw = glm::degrees(atan2(v.x, v.z));
		if (newYaw > 180.0f) newYaw -= 360.0f;
		if (newYaw < -180.0f) newYaw += 360.0f;
		if (modelController.Yaw > 180.0f) modelController.Yaw -= 360.0f;
		if (modelController.Yaw < -180.0f) modelController.Yaw += 360.0f;
		float deltaYaw = newYaw - modelController.Yaw;
		if (deltaYaw > 180.0f) deltaYaw -= 360.0f;
		if (deltaYaw < -180.0f) deltaYaw += 360.0f;
		modelController.Yaw += deltaYaw * deltaTime * 10.0f;
		modelController.updateVectors();
	}
	else {
		btVector3 cur = body->getLinearVelocity();
		body->setLinearVelocity(btVector3(0.0f, cur.getY(), 0.0f));
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
Animation* currentCatAnimation = nullptr;

void SetCatAnimation(Animation* newClip, AnimState newState)
{
	// si viene nullptr no hacemos nada
	if (!newClip) return;

	// Cambiar sólo si es un estado/clip distinto para NO reiniciar el timing
	if (newClip != gCurrentClip || newState != gCurrentState)
	{
		catAnimator1.PlayAnimation(newClip, /*resetIfNew=*/false); // <- reanuda
		gCurrentClip = newClip;
		gCurrentState = newState;
	}
}