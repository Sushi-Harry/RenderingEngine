#include "Scene.hpp"
#include "Framebuffer_PixelPicker.hpp"
#include "ModelLoader.hpp"
#include "PostProcessing_Framebuffer.hpp"
#include "ShaderClass.hpp"
#include "Skybox.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

char name[256] = "";
char pathToVS[512] = "";
char pathToFS[512] = "";
char pathToModel[512] = "";

SceneManager::SceneManager(GLFWwindow *window) {
    guiState = GENERAL_CONTROLS;
    SetupObjects(window);
}

void SceneManager::SetupObjects(GLFWwindow *window) {
    skybox = new Skybox();
    mainGrid = new Grid();
    // Selection Framebuffer object
    glfwGetWindowSize(window, &WINDOW_WIDTH, &WINDOW_HEIGHT);
    selection_framebuffer = new SELECTION_FRAMEBUFFER(WINDOW_WIDTH, WINDOW_HEIGHT);
    stencilBufferShader = new Shader("include/Essentials/ImpShaders/selectionMode.vs","include/Essentials/ImpShaders/stencilBuffer.fs");

    LitModels.clear();
    DirectionalLights.clear();
    PointLights.clear();
    SpotLights.clear();
    postProcessingStack.clear();
    GUI_INIT(window);
}

void SceneManager::CreateDirLight(std::string NAME) {
    DirectionalLight newDirLight;
    newDirLight.setName(NAME);
    DirectionalLights.push_back(newDirLight);
}

void SceneManager::CreateSpotLight(std::string NAME) {
    SpotLight newSpotLight;
    newSpotLight.setName(NAME);
    SpotLights.push_back(newSpotLight);
}

void SceneManager::CreatePointLight(std::string NAME) {
    PointLight newPointLight;
    newPointLight.setName(NAME);
    PointLights.push_back(newPointLight);
}

void SceneManager::CreateLitModel(std::string NAME, std::string PATH_TO_MODEL,
                                  std::string VS_PATH, std::string FS_PATH) {
    Model litModel(PATH_TO_MODEL, VS_PATH, FS_PATH, NAME);
    LitModels.push_back(litModel);
}

void SceneManager::AddPostProcessingEffect(std::string NAME, std::string PATH_TO_SHADER){
    
    POSTPROCESSING_BUFFER newEffect(WINDOW_WIDTH, WINDOW_HEIGHT, NAME, PATH_TO_SHADER);
    postProcessingStack.push_back(newEffect);
}

void SceneManager::GUI_INIT(GLFWwindow *window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void SceneManager::DrawGUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("MENUS");
    /*
        TO HANDLE MULTIPLE WINDOW STATES, I'm gonna be using an enum that acts
       as a toggle of sorts
    */
    const char *enumNames[] = {"General", "Add Model", "Add Spot Light",
                               "Add Point Light", "Add Directional Light", "Add Post Processing Effect"};

    static GUI_STATE currentState = GENERAL_CONTROLS;
    if (ImGui::Combo("Options", (int *)&currentState, enumNames,
                     IM_ARRAYSIZE(enumNames))) {
        guiState = currentState;
    }

    switch (currentState) {
    case GENERAL_CONTROLS: {
        for (auto &object : DirectionalLights) {
            object.takeInput();
        }
        for (auto &object : PointLights) {
            object.takeInput();
        }
        for (auto &object : SpotLights) {
            object.takeInput();
        }
        for(auto &effect : postProcessingStack){
            effect.takeInput();
        }
    } break;

    case ADD_MODEL: {

        ImGui::InputText("MODEL NAME", name, IM_ARRAYSIZE(name));
        ImGui::InputText("PATH TO VS", pathToVS, IM_ARRAYSIZE(pathToVS));
        ImGui::InputText("PATH TO FS", pathToFS, IM_ARRAYSIZE(pathToFS));
        ImGui::InputText("MODEL PATH", pathToModel, IM_ARRAYSIZE(pathToModel));
        if (ImGui::Button("Create Model ")) {
            CreateLitModel(name, pathToModel, pathToVS, pathToFS);
            guiState = GENERAL_CONTROLS;
        }
    } break;
    case ADD_POINT_LIGHT: {
        ImGui::InputText("Point Light Name: ", name, IM_ARRAYSIZE(name));
        if (ImGui::Button("Create Point Light ")) {
            CreatePointLight(name);
            guiState = GENERAL_CONTROLS;
        }
    } break;

    case ADD_SPOT_LIGHT: {
        ImGui::InputText("Spot Light Name: ", name, IM_ARRAYSIZE(name));
        if (ImGui::Button("Create Spot Light ")) {
            CreateSpotLight(name);
            guiState = GENERAL_CONTROLS;
        }
    } break;
    case ADD_DIRECTIONAL_LIGHT: {
        ImGui::InputText("Directional Light Name; ", name, IM_ARRAYSIZE(name));
        if (ImGui::Button("Create Directional Light ")) {
            CreateDirLight(name);
            guiState = GENERAL_CONTROLS;
        }
    } break;
    case POST_PROCESSING:{
        ImGui::InputText("Effect Name: ", name, IM_ARRAYSIZE(name));
        ImGui::InputText("Fragment Shader for the Effect:  ", pathToFS, IM_ARRAYSIZE(pathToFS));
        if(ImGui::Button("Add Effect")){
            AddPostProcessingEffect(name, pathToFS);
            guiState = GENERAL_CONTROLS;
            std::cout << "EFFECT ADDED" << std::endl;
        };

    }break;
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void SceneManager::PreUpdate() {
    // This function assigns (and reassigns the IDs to the LitModels array members)
    for (int i = 1; i <= LitModels.size(); i++) {
        LitModels[i-1].SetID(i);
    }
}

void SceneManager::ProcessMouseClick(GLFWwindow *WINDOW, int WIDTH, int HEIGHT, glm::mat4 PROJECTION, glm::mat4 VIEW_MATRIX, glm::vec3 VIEW_POSITION) {
    // This function should only be called when a mouse button is pressed.
    
    // Step 1: Render the scene with unique colors to the picking framebuffer.
    selection_framebuffer->RENDER_TO_FBO(PROJECTION, VIEW_MATRIX, VIEW_POSITION, WIDTH, HEIGHT, LitModels);
    
    // Step 2: Read the pixel at the cursor's position.
    unsigned int pickedID = selection_framebuffer->READ_PICKED_ID(WINDOW, WIDTH, HEIGHT);
    printf("The picked id is: %d \n", pickedID);

    // Step 3: Update the selected models.
    for (auto &model : LitModels) {
        if (model.ID == pickedID) {
            // Toggle selection: if it's already selected, deselect it.
            model.isSelected = true;
        } else {
            // Deselect all other models (if you don't want multi-select)
            model.isSelected = false;
        }
    }
}


void SceneManager::ResultantDrawScene(glm::mat4 PROJECTION,
                                      glm::mat4 VIEW_MATRIX,
                                      glm::vec3 VIEW_POSITION, int WIDTH,
                                      int HEIGHT, GLFWwindow *WINDOW) {

    // The slow picking code has been moved to ProcessMouseClick()
    // The Input Part of the Function:
    if(glfwGetMouseButton(WINDOW, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
        ProcessMouseClick(WINDOW, WIDTH, HEIGHT, PROJECTION, VIEW_MATRIX, VIEW_POSITION);
    }

    // --- Main Render Pass ---
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Ensure we're drawing to the screen
    glViewport(0, 0, WIDTH, HEIGHT);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // --- FIX 6: Keep Stencil Test enabled for the whole outlining process ---
    glEnable(GL_STENCIL_TEST);
    // Clear all buffers at the start of the frame
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    // 1. Draw the scene normally (skybox, grid, etc.)
    glStencilMask(0x00); // Disable writing to the stencil buffer
    glEnable(GL_DEPTH_TEST); // Re-enable depth testing
    if(!postProcessingStack.empty()){
        for(int i = 0; i<postProcessingStack.size(); i++){
            if(postProcessingStack[i].ENABLED)
                postProcessingStack[i].PostProcessing_START();
        }
    }
    glDepthFunc(GL_LESS);

    skybox->DrawSkybox(PROJECTION, VIEW_MATRIX);
    mainGrid->DrawGrid(PROJECTION, VIEW_MATRIX, VIEW_POSITION);
    
    // Set light uniforms once
    for (auto &model : LitModels) {
        model.ModelShader->use();
        for (auto &dirLight : DirectionalLights) dirLight.SetShader(*model.ModelShader);
        for (auto &pointLight : PointLights) pointLight.SetShader(*model.ModelShader);
        for (auto &spotLight : SpotLights) spotLight.SetShader(*model.ModelShader);
    }
    // Draw all models
    for(auto &model : LitModels){
        model.Draw(PROJECTION, VIEW_MATRIX, VIEW_POSITION, 1.0f);
    }

    
    // 2. Render the selected object(s) into the stencil buffer.
    glStencilFunc(GL_ALWAYS, 1, 0xFF); // Stencil test always passes
    glStencilMask(0xFF); // Enable writing to the stencil buffer
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); // Replace stencil value with 1
    glDepthFunc(GL_LEQUAL);
    
    for(auto &model : LitModels){
        if(model.isSelected) {
            model.Draw(PROJECTION, VIEW_MATRIX, VIEW_POSITION, 1.0f);
        }
    }
    
    glDepthFunc(GL_LESS);

    // 3. Draw the scaled-up outline where the stencil buffer is not 1.
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // Only draw where stencil value is NOT 1
    glStencilMask(0x00); // Disable writing to the stencil buffer again
    glDisable(GL_DEPTH_TEST); // Disable depth test to see outline through other objects
    
    for (auto &model : LitModels) {
        if(model.isSelected){
            model.Draw_NO_SHADER_USE(PROJECTION, VIEW_MATRIX, VIEW_POSITION, 1.01f, *stencilBufferShader);
        }
    }
    
    // 4. Reset state for the next frame
    glStencilMask(0xFF); // Re-enable stencil writing for clearing next frame
    glStencilFunc(GL_ALWAYS, 1, 0xFF); // Reset stencil function
    glEnable(GL_DEPTH_TEST); // Re-enable depth test
    glDisable(GL_STENCIL_TEST); // Can disable here until next frame
    
    if(!postProcessingStack.empty()){
        for(int i = postProcessingStack.size() - 1; i>=0; i--){
            if(postProcessingStack[i].ENABLED)
                postProcessingStack[i].PostProcessing_END();
        }
    }
    
    DrawGUI();
}