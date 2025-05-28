#ifndef CALLBACKS_H
#define CALLBACKS_H

#include "Globals.h"


extern void framebuffer_size_callback(GLFWwindow* window, int width, int height);
extern void mouse_callback(GLFWwindow* window, double xpos, double ypos);
extern scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
extern processInput(GLFWwindow* window);

#endif
