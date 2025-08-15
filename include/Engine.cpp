#include "Engine.hpp"
#include "ShaderClass.hpp"
#include "camera.h"
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Engine::Engine(int HEIGHT, int WIDTH, const char *WINDOW_NAME) {

    // For later use
    height = HEIGHT;
    width = WIDTH;

    CURSOR_HIDDEN = true;
    TAB_WAS_PRESSED = false;

    // initialize glfw
    if (!initGLFW()) {
        std::cout << "Couldn't initialize GLFW\n" << std::endl;
        return;
    }

    // Create a winndow
    window = glfwCreateWindow(WIDTH, HEIGHT, WINDOW_NAME, NULL, NULL);
    if (!window) {
        std::cout << "Couldn't initialize the window" << std::endl;
        return;
    }
    glfwMakeContextCurrent(window);

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, Engine::framebuffer_size_callback);
    glfwSetCursorPosCallback(window, Engine::mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    // initialize GLEW
    if (!initGLEW()) {
        std::cout << "Couldn't Initialize GLEW\n" << std::endl;
        return;
    }
    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSwapInterval(1);

    //---------------------MAP-----------------------------//
    sceneMgr = new SceneManager(window);
    //-------------------CAMERA----------------------------//
    CameraSetup();
    //--------------------Selection Mode Stuff---------------------//
    BufferSetup();
}

void Engine::framebuffer_size_callback(GLFWwindow *WINDOW, int WIDTH,
                                       int HEIGHT) {
    Engine *engineInstance =
        static_cast<Engine *>(glfwGetWindowUserPointer(WINDOW));
    if (engineInstance) {
        glViewport(0, 0, WIDTH, HEIGHT);
    }
}
void Engine::mouse_callback(GLFWwindow *window, double xposIn, double yposIn) {
    Engine *engineInstance =
        static_cast<Engine *>(glfwGetWindowUserPointer(window));
    if (engineInstance && engineInstance->CURSOR_HIDDEN) {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (engineInstance->firstMouse) {
            engineInstance->lastX = xpos;
            engineInstance->lastY = ypos;
            engineInstance->firstMouse = false;
        }
        float xOffset = xpos - engineInstance->lastX;
        float yOffset = engineInstance->lastY - ypos;

        engineInstance->lastX = xpos;
        engineInstance->lastY = ypos;
        engineInstance->MainCamera->ProcessMouseMovement(xOffset, yOffset);
    }
}

bool Engine::initGLFW() {
    if (!glfwInit()) {
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    return true;
    ;
}

bool Engine::initGLEW() {
    glewExperimental = GL_TRUE;
    return glewInit() == GLEW_OK;
}

bool Engine::isRunning() {
    return !glfwWindowShouldClose(window);
}

void Engine::CameraSetup() {
    MainCamera = new Camera(glm::vec3(0.0f, -0.5f, 4.0f));

    lastX = width / 2.0f;
    lastY = height / 2.0f;
    firstMouse = true;

    deltaTime = 0.0f;
    lastFrame = 0.0f;
}

void Engine::DeltaTimeCalculation() {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

void Engine::ProcessMovement(GLFWwindow *window) {

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // THIS IS THE CODE FOR MOUSE TOGGLE
    bool tabPressed = glfwGetKey(window, GLFW_KEY_GRAVE_ACCENT) == GLFW_PRESS;
    if (tabPressed && !TAB_WAS_PRESSED) {
        if (CURSOR_HIDDEN) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        CURSOR_HIDDEN = !CURSOR_HIDDEN;
    }
    TAB_WAS_PRESSED = tabPressed;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        MainCamera->ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        MainCamera->ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        MainCamera->ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        MainCamera->ProcessKeyboard(RIGHT, deltaTime);
}

void Engine::Update() {
    if (glfwWindowShouldClose(window)) {
        return;
    }
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
    // GL_STENCIL_BUFFER_BIT);

    DeltaTimeCalculation();

    // Test code for model
    glm::mat4 projection = glm::perspective(
        glm::radians(90.0f), (float)width / (float)height, 0.1f, 100.0f);
    glm::mat4 view = MainCamera->GetViewMatrix();
    //--------------------------DRAW USING SCENE MANAGER
    // CLASS-------------------------------//
    // sceneMgr->PreUpdate();
    sceneMgr->ResultantDrawScene(projection, view, MainCamera->Position, width,
                                 height, window);

    ProcessMovement(window);
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void Engine::BufferSetup() {
    SelectionShader =
        new Shader("include/Essentials/ImpShaders/selectionMode.vs",
                   "include/Essentials/ImpShaders/selectionMode.fs");
}

void Engine::PostUpdate() {}
