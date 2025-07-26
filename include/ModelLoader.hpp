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
    void Draw(glm::mat4 PROJECTION, glm::mat4 VIEW_MATRIX, glm::vec3 VIEW_POS);
    Shader* ModelShader;
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

};