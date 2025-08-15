#pragma once

#include "ModelLoader.hpp"
#include "ShaderClass.hpp"
#include "lights.hpp"
#include <GL/glew.h>
#include <vector>

class StencilBuffer {

  public:
    StencilBuffer();
    void DrawStencilBuffer(std::vector<Model> RENDER_OBJECTS, int PICKED_ID);
    void DrawStencilBuffer(std::vector<Model> MODEL_ARRAY,
                           unsigned int PICKED_ID, glm::mat4 PROJECTION,
                           glm::mat4 VIEW, glm::vec3 VIEW_POS);
    void DrawOutlined(Model MODEL, glm::mat4 PROJECTION, glm::mat4 VIEW_MATRIX,
                      std::vector<PointLight> POINT_LIGHTS,
                      std::vector<DirectionalLight> DIRECTIONAL_LIGHTS,
                      std::vector<SpotLight> SPOT_LIGHTS, glm::vec3 VIEW_POS);

  private:
    Shader *stencilShader;
};
