#include "Scene.hpp"
#include "ModelLoader.hpp"
#include "Skybox.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include <string>

char name[256] = "";
char pathToVS[512] = "";
char pathToFS[512] = "";
char pathToModel[512] = "";


SceneManager::SceneManager(GLFWwindow* window){
    guiState = GENERAL_CONTROLS;
    SetupObjects(window);
}

void SceneManager::SetupObjects(GLFWwindow* window){
    skybox = new Skybox();
    mainGrid = new Grid();
    LitModels.clear();
    DirectionalLights.clear();
    PointLights.clear();
    SpotLights.clear();
    GUI_INIT(window);
}

void SceneManager::CreateDirLight(std::string NAME){
    DirectionalLight newDirLight;
    newDirLight.setName(NAME);
    DirectionalLights.push_back(newDirLight);
}

void SceneManager::CreateSpotLight(std::string NAME){
    SpotLight newSpotLight;
    newSpotLight.setName(NAME);
    SpotLights.push_back(newSpotLight);
}

void SceneManager::CreatePointLight(std::string NAME){
    PointLight newPointLight;
    newPointLight.setName(NAME);
    PointLights.push_back(newPointLight);   
}

void SceneManager::CreateLitModel(std::string NAME, std::string PATH_TO_MODEL, std::string VS_PATH, std::string FS_PATH){
    Model litModel(PATH_TO_MODEL, VS_PATH, FS_PATH, NAME);
    LitModels.push_back(litModel);
}

void SceneManager::GUI_INIT(GLFWwindow* window){
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void SceneManager::DrawGUI(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("MENUS");
        /*
            TO HANDLE MULTIPLE WINDOW STATES, I'm gonna be using an enum that acts as a toggle of sorts
        */
        const char* enumNames[] = {
            "General",
            "Add Model",
            "Add Point Light",
            "Add Spot Light",
            "Add Directional Light"
        };

        static GUI_STATE currentState = GENERAL_CONTROLS;
        if(ImGui::Combo("Options", (int*)&currentState, enumNames, IM_ARRAYSIZE(enumNames))){
            guiState = currentState;
        }

        switch (currentState) {
            case GENERAL_CONTROLS:{
                    for(auto &object : DirectionalLights){
                        object.takeInput();
                    }
                    for (auto &object : PointLights) {
                        object.takeInput();
                    }
                    for (auto &object : SpotLights) {
                        object.takeInput();
                    }
            }
            break;

            case ADD_MODEL:{
                ImGui::InputText("MODEL NAME", name, IM_ARRAYSIZE(name));
                ImGui::InputText("PATH TO VS", pathToVS, IM_ARRAYSIZE(pathToVS));
                ImGui::InputText("PATH TO FS", pathToFS, IM_ARRAYSIZE(pathToFS));
                ImGui::InputText("MODEL PATH", pathToModel, IM_ARRAYSIZE(pathToModel));
                if(ImGui::Button("Create Model ")){
                    CreateLitModel(name, pathToModel, pathToVS, pathToFS);
                    guiState = GENERAL_CONTROLS;
                }
            }
            break;
            case ADD_POINT_LIGHT:{
                ImGui::InputText("Point Light Name: ", name, IM_ARRAYSIZE(name));
                if(ImGui::Button("Create Point Light ")){
                    CreatePointLight(name);
                    guiState = GENERAL_CONTROLS;
                }
            }
            break;

            case ADD_SPOT_LIGHT:{
                ImGui::InputText("Spot Light Name: ", name, IM_ARRAYSIZE(name));
                if(ImGui::Button("Create Spot Light ")){
                    CreateSpotLight(name);
                    guiState = GENERAL_CONTROLS;
                }
            }
            break;
            case ADD_DIRECTIONAL_LIGHT:{
                ImGui::InputText("Directional Light Name; ", name, IM_ARRAYSIZE(name));
                if(ImGui::Button("Create Directional Light ")){
                    CreateDirLight(name);
                    guiState = GENERAL_CONTROLS;
                }
            }
            break;
        }
        
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void SceneManager::DrawScene(glm::mat4 PROJECTION, glm::mat4 VIEW_MATRIX, glm::vec3 VIEW_POS){
    skybox->DrawSkybox(PROJECTION, VIEW_MATRIX);
    mainGrid->DrawGrid(PROJECTION, VIEW_MATRIX, VIEW_POS);
    // Setting shader values
    for (auto &model : LitModels) {
        model.ModelShader->use();
        if(!PointLights.empty()){
            for(auto &pntLight : PointLights){
                pntLight.SetShader(*model.ModelShader);
            }
        }
        if(!DirectionalLights.empty()){
            for(auto &dirLight : DirectionalLights){
                dirLight.SetShader(*model.ModelShader);
            }
        }
        if(!SpotLights.empty()){
            for(auto &sptLight : SpotLights){
                sptLight.SetShader(*model.ModelShader);
            }
        }
    }
    for(auto &model: LitModels){
        model.Draw(PROJECTION, VIEW_MATRIX, VIEW_POS);
    }

    DrawGUI();
}