#pragma once
#include "ModelLoader.hpp"
#include "ShaderClass.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class SELECTION_FRAMEBUFFER {
  public:
    SELECTION_FRAMEBUFFER(int WIDTH, int HEIGHT);
    ~SELECTION_FRAMEBUFFER();
    glm::vec3 OBJECT_ID_TO_COLOR(unsigned int ID);
    unsigned int COLOR_TO_OBJECT_ID(unsigned char color[3]);
    void RENDER_TO_FBO(glm::mat4 PROJECTION, glm::mat4 VIEW_MATRIX,
                                          glm::vec3 VIEW_POS, int WINDOW_WIDTH, int WINDOW_HEIGHT,
                                          const std::vector<Model>& MODEL_ARRAY);
    unsigned int READ_PICKED_ID(GLFWwindow* WINDOW, int WINDOW_WIDTH, int WINDOW_HEIGHT);
    void RUNTIME_FUNCTION();

  private:
    Shader *FLAT_SHADER;
    unsigned int FBO, DEPTH_RBO;
    unsigned int TEXTURE;
};
