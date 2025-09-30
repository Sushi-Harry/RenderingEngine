#pragma once

#include "ShaderClass.hpp"
class POSTPROCESSING_BUFFER{
public:
    POSTPROCESSING_BUFFER(int WINDOW_WIDTH, int WINDOW_HEIGHT, std::string NAME, std::string PATH_TO_SHADER);
    void PostProcessing_START();
    void PostProcessing_END();
    ~POSTPROCESSING_BUFFER();
    void toggleEffect();
    void takeInput();
    bool ENABLED;
private:
    //This function sets up the screen quadrilateral data
    void SetupScreenQuad();

    std::string NAME;
    unsigned int FBO, RBO;
    unsigned int TEXTURE;
    Shader* SHADER;

    // For the screen quad
    unsigned int VAO, VBO, EBO;
};