#include <vector>
#include "ModelLoader.hpp"
#include "ShaderClass.hpp"
#include "Skybox.hpp"
#include "lights.hpp"
#include "Grid.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

enum GUI_STATE {
    GENERAL_CONTROLS,
    ADD_MODEL,
    ADD_SPOT_LIGHT,
    ADD_POINT_LIGHT,
    ADD_DIRECTIONAL_LIGHT
};

class SceneManager{
public:
    SceneManager(GLFWwindow* window);
        void DrawScene(glm::mat4 PROJECTION, glm::mat4 VIEW_MATRIX, glm::vec3 VIEW_POS);
    ~SceneManager();
private:
    void SetupObjects(GLFWwindow* window);

    void CreateDirLight(std::string NAME);
    void CreateSpotLight(std::string NAME);
    void CreatePointLight(std::string NAME);
    void GUI_INIT(GLFWwindow* window);
    void DrawGUI();

    void CreateLitModel(std::string NAME, std::string PATH_TO_MODEL, std::string VS_PATH, std::string FS_PATH);

    Skybox* skybox;
    GUI_STATE guiState;
    std::vector<Model> LitModels;
    std::vector<DirectionalLight> DirectionalLights;
    std::vector<PointLight> PointLights;
    std::vector<SpotLight> SpotLights;
    Grid* mainGrid; 
    // Test Variables   
    Model* testModel;
    DirectionalLight* Light1;
};