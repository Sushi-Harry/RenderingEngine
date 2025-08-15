#pragma once

#include "Mesh.hpp"
#include "ShaderClass.hpp"
#include <assimp/material.h>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>

#include "stb_image.h"

class Model{

public:
    Model(const std::string &PATH, const std::string &SHADER_PATH_VS, const std::string &SHADER_PATH_FS, std::string NAME);
    void Draw(glm::mat4 PROJECTION, glm::mat4 VIEW_MATRIX, glm::vec3 VIEW_POS, float ScaleFactor);
    void Draw_SELECTIONMODE(glm::mat4 PROJECTION, glm::mat4 VIEW_MATRIX, glm::vec3 VIEW_POS, float ScaleFactor);
    void scaleModel(float scaleFactor, glm::mat4& model_matrix);
    void Draw_NO_SHADER_USE(glm::mat4 PROJECTION, glm::mat4 VIEW_MATRIX, glm::vec3 VIEW_POS, float ScaleFactor, Shader& CUSTOM_SHADER);
    Shader* ModelShader;
    void SetID(int ID);

    glm::vec3 color;
    unsigned int ID;
private:
    // Basics
    std::string NAME;

    // Model Data
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;

    void LoadModel(std::string path);
    void ProcessNode(aiNode* node, const aiScene* scene);
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

    Shader* SELECTION_SHADER;

};