#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext/vector_uint4.hpp>
#include "camera.h"
#include "Scene.hpp"

class Engine{
public:
    Engine(int HEIGHT, int WIDTH, const char* WINDOW_NAME);
    static void framebuffer_size_callback(GLFWwindow* WINDOW, int WIDTH, int HEIGHT);
    static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);

    void setupShader(const char* VERTEX_SHADER, const char* FRAGMENT_SHADER);

    void Update();
    bool isRunning();
private:
    GLFWwindow* window;
    Camera* MainCamera;

    //----------------------------MAP CLASS----------------------------------//
    SceneManager* sceneMgr;

    // Input Variables
    bool CURSOR_HIDDEN;
    bool TAB_WAS_PRESSED;

    int width, height; // WINDOW VARIABLES
    // Delta Time Implementation
    float lastX, lastY;
    bool firstMouse;
    float deltaTime;
    float lastFrame;

    // Private Functions
    bool initGLFW();
    bool initGLEW();
    void CameraSetup();
    void DeltaTimeCalculation();
    void ProcessMovement(GLFWwindow* window);
};