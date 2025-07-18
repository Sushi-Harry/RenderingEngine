#include "Map.hpp"
#include "ShaderClass.hpp"
#include "Skybox.hpp"
#include "camera.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Map::Map(GLFWwindow* window){

    grid = new Grid();

    DirectionalLights.clear();
    PointLights.clear();
    SpotLights.clear();
    Shaders.clear();

    Model* cyborgModel = new Model("models/cyborg/cyborg.obj");
    Model* sunModel = new Model("models/sphere/sphere.obj");
    Model* flatSurface = new Model("models/FlatPlane/flatPlane.obj");

    Models.push_back(cyborgModel);
    Models.push_back(sunModel);
    Models.push_back(flatSurface);

    skybox = new Skybox();

    cyborgShader = new Shader("shaders/cyborgShader.vs", "shaders/cyborgShader.fs");
    sunShader = new Shader("shaders/sun.vs", "shaders/sun.fs");
    SkyboxShader = new Shader("shaders/Skybox.vs", "shaders/Skybox.fs");
    flatSurfaceShader = new Shader("shaders/flatSurface.vs", "shaders/flatSurface.fs");

    //-----------------------------------Lights--------------------------------------------------//
    DirLight = new DirectionalLight();
    PointLight1 = new PointLight();
    PointLight1->color = glm::vec3(1.0f, 0.81f, 0.40f);
    PointLight1->position = glm::vec3(0.0f, 0.0f, 3.0f);

    SpotLight1 = new SpotLight();
    
    CYBORG_POSITION = glm::vec3(0.0f);
    
    //----------------------------------ImGUI INitialization-----------------------------------------//
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void Map::GUI(){
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("CONTROLS");
        // Lighting Controls
        DirLight->takeInput();
        PointLight1->takeInput();
        SpotLight1->takeInput();
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Map::DrawMap(glm::mat4 PROJECTION_MATRIX, glm::mat4 VIEW_MATRIX, glm::vec3 VIEW_POSITION){
    
    
    // The Sun is drawn first
    sunShader->use();
    glm::mat4 model_sun = glm::mat4(1.0f);
    model_sun = glm::translate(model_sun, PointLight1->position);
    model_sun = glm::scale(model_sun, glm::vec3(0.005f));
    
    sunShader->setMat4("projection", PROJECTION_MATRIX);
    sunShader->setMat4("view", VIEW_MATRIX);
    sunShader->setMat4("model", model_sun);
    sunModel->Draw(*sunShader);
    
    // Then Draw Cyborg
    cyborgShader->use();
    glm::mat4 model_cyborg = glm::mat4(1.0f);
    model_cyborg = glm::translate(model_cyborg, CYBORG_POSITION);
    model_cyborg = glm::scale(model_cyborg, glm::vec3(1.0f));
    
    cyborgShader->setMat4("projection", PROJECTION_MATRIX);
    cyborgShader->setMat4("view", VIEW_MATRIX);
    cyborgShader->setMat4("model", model_cyborg);
    cyborgShader->setVec3("viewPos", VIEW_POSITION);
    cyborgShader->setFloat("material.shininess", 32.0f);

    // Draw the plane
    flatSurfaceShader->use();
    glm::mat4 model_flatPlane = glm::mat4(1.0f);
    model_flatPlane = glm::scale(model_flatPlane, glm::vec3(0.5f));
    model_flatPlane = glm::translate(model_flatPlane, glm::vec3(0.0f));
    flatSurfaceShader->setMat4("projection", PROJECTION_MATRIX);
    flatSurfaceShader->setMat4("view", VIEW_MATRIX);
    flatSurfaceShader->setMat4("model", model_flatPlane);
    flatSurfaceShader->setVec3("viewPos", VIEW_POSITION);
    flatSurfaceShader->setFloat("material.shininess", 32.0f);

    
    // POINT LIGHT
    PointLight1->SetShader(*cyborgShader);
    // DIRECTIONAL LIGHT
    DirLight->SetShader(*cyborgShader);
    // SPOT LIGHT
    SpotLight1->SetShader(*cyborgShader);
    // Draw the cyborg
    // cyborgModel->Draw(*cyborgShader);
    // Draw the plane
    flatSurface->Draw(*flatSurfaceShader);
    // Grid --------- DISABLE FACE CULLING FIRST AND THEN RE-ENABLE CULLING AFTER DRAWING THE GRID ------------- // 
    glDisable(GL_CULL_FACE);
    grid->DrawGrid(PROJECTION_MATRIX, VIEW_MATRIX, VIEW_POSITION);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    // Skybox
    skybox->DrawSkybox(*SkyboxShader, PROJECTION_MATRIX, VIEW_MATRIX);
}

Map::~Map(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    delete cyborgModel;
    delete sunModel;
    delete flatSurface;
    delete cyborgShader;
    delete sunShader;
    delete flatSurfaceShader;
}