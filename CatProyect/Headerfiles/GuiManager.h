#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

class GuiManager {
public:
    void Init(GLFWwindow* window);
    void Shutdown();

    void RenderStartScreen(bool& showStartScreen, bool& fadeOut, float& fadeOpacity);
};

#endif