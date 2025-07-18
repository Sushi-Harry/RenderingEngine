#pragma once

#include "camera.h"
#include "GL/glew.h"
#include "ShaderClass.hpp"


#include <string>
#include <vector>

class Skybox{
public:
    Skybox();
    ~Skybox();

    void drawSetup();
    unsigned int loadCubeMap(std::vector<std::string> faces);

    void DrawSkybox(Shader &shader, glm::mat4 proection, glm::mat4 view);
    unsigned cubemapTexture;

private:
    unsigned int VAO, VBO;
};