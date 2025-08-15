#include "StencilBuffer.hpp"
#include "ShaderClass.hpp"
#include "lights.hpp"
#include <glm/ext/matrix_float4x4.hpp>
#include <vector>

StencilBuffer::StencilBuffer() {
    stencilShader =
        new Shader("include/Essentials/ImpShaders/selectionMode.vs",
                   "include/Essentials/ImpShaders/stencilBuffer.vs");
}

void StencilBuffer::DrawOutlined(
    Model MODEL, glm::mat4 PROJECTION, glm::mat4 VIEW_MATRIX,
    std::vector<PointLight> POINT_LIGHTS,
    std::vector<DirectionalLight> DIRECTIONAL_LIGHTS,
    std::vector<SpotLight> SPOT_LIGHTS, glm::vec3 VIEW_POS) {
    glEnable(GL_DEPTH_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);

    // Drawing the model that has to be outlined
    if (!DIRECTIONAL_LIGHTS.empty()) {
        for (auto &dirLight : DIRECTIONAL_LIGHTS) {
            dirLight.SetShader(*MODEL.ModelShader);
        }
    }
    if (!POINT_LIGHTS.empty()) {
        for (auto &pntLight : POINT_LIGHTS) {
            pntLight.SetShader(*MODEL.ModelShader);
        }
    }
    if (!SPOT_LIGHTS.empty()) {
        for (auto &spotLight : SPOT_LIGHTS) {
            spotLight.SetShader(*MODEL.ModelShader);
        }
    }
    // Draw the model after setting the uniforms
    MODEL.Draw(PROJECTION, VIEW_MATRIX, VIEW_POS, 1.0f);
    // END OF 1st RENDER PASS
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);
    MODEL.Draw_NO_SHADER_USE(PROJECTION, VIEW_MATRIX, VIEW_POS, 1.01f,
                             *stencilShader);
}

void StencilBuffer::DrawStencilBuffer(std::vector<Model> MODEL_ARRAY,
                                      unsigned int PICKED_ID,
                                      glm::mat4 PROJECTION, glm::mat4 VIEW,
                                      glm::vec3 VIEW_POS) {
    glEnable(GL_DEPTH_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
    // GL_STENCIL_BUFFER_BIT);

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    for (auto &model : MODEL_ARRAY) {
        if (model.ID == PICKED_ID) {
            model.ModelShader->use();
            model.Draw(PROJECTION, VIEW, VIEW_POS, 1.0f);
        } else {
            continue;
        }
    }

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);
    stencilShader->use();
    for (auto &model : MODEL_ARRAY) {
        if (model.ID == PICKED_ID) {
            model.Draw_NO_SHADER_USE(PROJECTION, VIEW, VIEW_POS, 1.1f,
                                     *stencilShader);
        }
    }
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glEnable(GL_DEPTH_TEST);
}
