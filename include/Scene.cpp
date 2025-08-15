#include "Scene.hpp"
#include "ModelLoader.hpp"
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
    stencilBufferShader =
        new Shader("include/Essentials/ImpShaders/selectionMode.vs",
                   "include/Essentials/ImpShaders/stencilBuffer.fs");
    LitModels.clear();
    DirectionalLights.clear();
    DirectionalLights.push_back(DirectionalLight());
    PointLights.clear();
    SpotLights.clear();
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
    const char *enumNames[] = {"General", "Add Model", "Add Point Light",
                               "Add Spot Light", "Add Directional Light"};

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
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void SceneManager::PreUpdate() {
    for (int i = 0; i < LitModels.size(); i++) {
        LitModels[i].SetID(i);
    }
}

unsigned int SceneManager::SelectionBuffer(glm::mat4 PROJECTION,
                                           glm::mat4 VIEW_MATRIX,
                                           glm::vec3 VIEW_POS, int WIDTH,
                                           int HEIGHT, GLFWwindow *WINDOW) {

    // Convert ID to Color
    for (auto &model : LitModels) {
        model.color.r = (model.ID & 0x0000FF) / 255.0f;
        model.color.g = ((model.ID & 0x00FF00) >> 8) / 255.0f;
        model.color.b = ((model.ID & 0xFF0000) >> 16) / 255.0f;
    }

    // Draw Scene with Color
    // No need to draw extra stuff like the skybox and the grid as they can't be
    // selected. Only draw the models Since that's what I'm trying to select

    if (!LitModels.empty()) {
        for (auto &model : LitModels) {
            model.Draw_SELECTIONMODE(PROJECTION, VIEW_MATRIX, VIEW_POS, 1.0f);
        }
        // Get Color Under Mouse
        glFlush();
        glFinish();
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        unsigned char data[4];
        if (glfwGetInputMode(WINDOW, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) {
            double x = 0.0, y = 0.0;
            glfwGetCursorPos(WINDOW, &x, &y);
            glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        // Color to ID
        int PickedID = data[0] + data[1] * 256 + data[2] * 256 * 256;
        // Return ID
        return PickedID;
    } else {
        return -1;
    }
}

void SceneManager::DrawScene(glm::mat4 PROJECTION, glm::mat4 VIEW_MATRIX,
                             glm::vec3 VIEW_POS, int WIDTH, int HEIGHT,
                             GLFWwindow *WINDOW) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    skybox->DrawSkybox(PROJECTION, VIEW_MATRIX);
    mainGrid->DrawGrid(PROJECTION, VIEW_MATRIX, VIEW_POS);
    // Setting shader values
    for (auto &model : LitModels) {
        model.ModelShader->use();
        if (!PointLights.empty()) {
            for (auto &pntLight : PointLights) {
                pntLight.SetShader(*model.ModelShader);
            }
        }
        if (!DirectionalLights.empty()) {
            for (auto &dirLight : DirectionalLights) {
                dirLight.SetShader(*model.ModelShader);
            }
        }
        if (!SpotLights.empty()) {
            for (auto &sptLight : SpotLights) {
                sptLight.SetShader(*model.ModelShader);
            }
        }
    }

    int PickedID = 32768;
    if (glfwGetMouseButton(WINDOW, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        PickedID = SelectionBuffer(PROJECTION, VIEW_MATRIX, VIEW_POS, WIDTH,
                                   HEIGHT, WINDOW);
    } else {
        PickedID = -11;
    }
    if (!LitModels.empty()) {
        std::cout << PickedID << "\n";
        std::cout << LitModels[0].ID << std::endl;
    }

    for (auto &model : LitModels) {
        if (model.ID != PickedID) {
            model.Draw(PROJECTION, VIEW_MATRIX, VIEW_POS, 1.0f);
        }
    }

    for (auto &model : LitModels) {
        if (model.ID == PickedID) {
            stencilBuffer->DrawOutlined(model, PROJECTION, VIEW_MATRIX,
                                        PointLights, DirectionalLights,
                                        SpotLights, VIEW_POS);
        }
    }

    DrawGUI();
}

void SceneManager::debug_drawOutlined(glm::mat4 PROJECTION,
                                      glm::mat4 VIEW_MATRIX, glm::vec3 VIEW_POS,
                                      int WIDTH, int HEIGHT,
                                      GLFWwindow *WINDOW) {

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // 1st Render Pass
    // These objects are not to be written to the stencil buffer
    glStencilMask(0x00);
    skybox->DrawSkybox(PROJECTION, VIEW_MATRIX);
    mainGrid->DrawGrid(PROJECTION, VIEW_MATRIX, VIEW_POS);
    // These will be written to the stencil buffer
    for (auto &model : LitModels) {
        model.ModelShader->use();
        if (!DirectionalLights.empty()) {
            for (auto &dirLight : DirectionalLights) {
                dirLight.SetShader(*model.ModelShader);
            }
        }
        if (!SpotLights.empty()) {
            for (auto &sptLight : SpotLights) {
                sptLight.SetShader(*model.ModelShader);
            }
        }
        if (!PointLights.empty()) {
            for (auto &pntLight : PointLights) {
                pntLight.SetShader(*model.ModelShader);
            }
        }
    }
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    for (auto &model : LitModels) {
        model.Draw(PROJECTION, VIEW_MATRIX, VIEW_POS, 1.0f);
    }

    // 2nd render pass
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00); // Stop Modifying the buffer
    for (auto &model : LitModels) {
        model.Draw_NO_SHADER_USE(PROJECTION, VIEW_MATRIX, VIEW_POS, 1.0f,
                                 *stencilBufferShader);
    }
}

void SceneManager::ResultantDrawScene(glm::mat4 PROJECTION,
                                      glm::mat4 VIEW_MATRIX,
                                      glm::vec3 VIEW_POSITION, int WIDTH,
                                      int HEIGHT, GLFWwindow *WINDOW) {

    /*THIS CODE IS SLOW AS FUCK!!!!!!!11!!1!!!!!!*/

    /* Clear THe buffer before drawing anything */
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    /* Draw the background objects without modifying the stencil buffer */
    glStencilMask(0x00);
    skybox->DrawSkybox(PROJECTION, VIEW_MATRIX);
    mainGrid->DrawGrid(PROJECTION, VIEW_MATRIX, VIEW_POSITION);

    /*Set the shaders for the models*/
    for (auto &model : LitModels) {
        model.ModelShader->use();
        for (auto &dirLight : DirectionalLights) {
            dirLight.SetShader(*model.ModelShader);
        }
        for (auto &pntLight : PointLights) {
            pntLight.SetShader(*model.ModelShader);
        }
        for (auto &sptLight : SpotLights) {
            sptLight.SetShader(*model.ModelShader);
        }
    }

    /* 1st Render pass: Draw the objects that have to be drawn without being
     * outlined. These objects always pass the stencil test */
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    for (auto &model : LitModels) {
        model.ModelShader->use();
        model.Draw(PROJECTION, VIEW_MATRIX, VIEW_POSITION, 1.0f);
    }

    /* 2nd Render Pass - Slightly scaled versions of the models */
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);

    for (auto &model : LitModels) {
        model.Draw_NO_SHADER_USE(PROJECTION, VIEW_MATRIX, VIEW_POSITION, 1.01f,
                                 *stencilBufferShader);
    }
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glEnable(GL_DEPTH_TEST);

    DrawGUI();
}
