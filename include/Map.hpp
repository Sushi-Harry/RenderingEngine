#pragma once

#include "ModelLoader.hpp"
#include "ShaderClass.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "Skybox.hpp"
#include "Grid.hpp"
#include "lights.hpp"
#include <vector>

class Map{

public:
    Map(GLFWwindow* window);
    ~Map();

    void DrawMap(glm::mat4 PROJECTION_MATRIX, glm::mat4 VIEW_MATRIX, glm::vec3 VIEW_POSITION);
    void GUI();

private:

    // Lights
    DirectionalLight* DirLight;
    PointLight* PointLight1;
    SpotLight* SpotLight1;

    Model* cyborgModel;
    Shader* cyborgShader;
    
    Skybox* skybox;
    Shader* SkyboxShader;

    Model* flatSurface;
    Shader* flatSurfaceShader;
    
    Model* sunModel;
    Shader* sunShader;

    std::vector<DirectionalLight*> DirectionalLights;
    std::vector<PointLight*> PointLights;
    std::vector<SpotLight*> SpotLights;

    std::vector<Model*> Models;
    std::vector<Shader*> Shaders;

    Grid* grid;
    
    glm::vec3 POINTLIGHT_POSITION;
    float POINTLIGHT_POSITION_FLOAT_ARRAY[3];

    glm::vec3 CYBORG_POSITION; // This is the object position in this case.

};