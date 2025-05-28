#ifndef GLOBALS_H
#define GLOBALS_H

#include <glm/glm.hpp>
#include "camera.h"
#include "animator.h"
#include "animation.h"
#include "model_animation.h"
#include "filesystem.h"
#include "Globals.h"

#include <FMOD/fmod.hpp>


// Tamaño de pantalla
extern unsigned int SCR_WIDTH;
extern unsigned int SCR_HEIGHT;
extern float lastX;
extern float lastY;

// Cámara y control de mouse
extern Camera camera;
extern bool firstMouse;

// Delta time
extern float deltaTime;
extern float lastFrame;

//// FMOD
//extern FMOD::System* fmodSystem;
//extern FMOD::Sound* startSound;
//extern FMOD::Sound* loopSound;
//extern FMOD::Channel* loopChannel;
//
//// Estados de UI
//extern bool runMain;
//extern bool startScreen;
//extern bool fadeOutStart;
//extern float fadeOpacity;
//
//// Animaciones
//extern Animation* cyborgAnimation;
//extern Animation* bananaAnimation;
//
//extern Animator cyborgAnimator;
//extern Animator bananaAnimator;

#endif
