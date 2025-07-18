#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "ShaderClass.hpp"


struct Vertex{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 textureCoords;
};

struct Texture{
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh{

public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex> VERTEX_VECTOR, std::vector<unsigned int> INDEX_VECTOR, std::vector<Texture> TEXTURE_VECTOR);
    void Draw(Shader &SHADER);
private:
    unsigned int VAO, VBO, EBO;

    void setupMesh();
};