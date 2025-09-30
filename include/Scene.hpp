#pragma once

#include "Framebuffer_PixelPicker.hpp"
#include "Grid.hpp"
#include "ModelLoader.hpp"
#include "PostProcessing_Framebuffer.hpp"
#include "ShaderClass.hpp"
#include "Skybox.hpp"
#include "lights.hpp"
#include <vector>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

enum GUI_STATE {
    GENERAL_CONTROLS,
    ADD_MODEL,
    ADD_SPOT_LIGHT,
    ADD_POINT_LIGHT,
    ADD_DIRECTIONAL_LIGHT,
    POST_PROCESSING
};

class SceneManager {
  public:
    SceneManager(GLFWwindow *window);

    void SelectionBuffer(glm::mat4 PROJECTION, glm::mat4 VIEW_MATRIX,
                         glm::vec3 VIEW_POS, int WIDTH, int HEIGHT,
                         GLFWwindow *WINDOW);

    void ResultantDrawScene(glm::mat4 PROJECTION, glm::mat4 VIEW_MATRIX,
                            glm::vec3 VIEW_POS, int WIDTH, int HEIGHT,
                            GLFWwindow *WINDOW);
    void PreUpdate();
    void ProcessMouseClick(GLFWwindow *WINDOW, int WIDTH, int HEIGHT, glm::mat4 PROJECTION, glm::mat4 VIEW_MATRIX, glm::vec3 VIEW_POSITION);
    ~SceneManager();

    SELECTION_FRAMEBUFFER* selection_framebuffer;

  private:
    void SetupObjects(GLFWwindow *window);

    void CreateDirLight(std::string NAME);
    void CreateSpotLight(std::string NAME);
    void CreatePointLight(std::string NAME);
    void AddPostProcessingEffect(std::string NAME, std::string PATH_TO_SHADER);
    void GUI_INIT(GLFWwindow *window);
    void DrawGUI();

    void CreateLitModel(std::string NAME, std::string PATH_TO_MODEL,
                        std::string VS_PATH, std::string FS_PATH);

    int WINDOW_WIDTH, WINDOW_HEIGHT;

    Skybox *skybox;
    GUI_STATE guiState;
    std::vector<Model> LitModels;
    std::vector<DirectionalLight> DirectionalLights;
    std::vector<PointLight> PointLights;
    std::vector<SpotLight> SpotLights;
    std::vector<POSTPROCESSING_BUFFER> postProcessingStack; 
    Grid *mainGrid;
    POSTPROCESSING_BUFFER* PostProcessing;

    // Framebuffer for Color Picking
    SELECTION_FRAMEBUFFER *FB_Selection;

    // Selection of object based on color
    unsigned int pickedModelID;
    // DEBUG
    Shader *stencilBufferShader;
};
