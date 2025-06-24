#ifndef INITGL_H
#define INITGL_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <btBulletDynamicsCommon.h>


extern unsigned int SCR_WIDTH;
extern unsigned int SCR_HEIGHT;

extern float lastX ;
extern float lastY ;


GLFWwindow* initOpenGL(bool fullscreen);
void processInput(GLFWwindow* window, btRigidBody* catRigidBody, float deltaTime, bool& enElSuelo);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


#endif
