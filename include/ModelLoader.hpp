#pragma once

#include "Mesh.hpp"
#include "ShaderClass.hpp"
#include <assimp/material.h>
#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <glm/glm.hpp>

#include "stb_image.h"

class Model {

  public:
    Model(const std::string &PATH, const std::string &SHADER_PATH_VS,
          const std::string &SHADER_PATH_FS, std::string NAME);
    void Draw(glm::mat4 PROJECTION, glm::mat4 VIEW_MATRIX, glm::vec3 VIEW_POS,
              float ScaleFactor) const;
    void Draw_SELECTIONMODE(glm::mat4 PROJECTION, glm::mat4 VIEW_MATRIX,
                            glm::vec3 VIEW_POS);
    void scaleModel(float scaleFactor, glm::mat4 &model_matrix) const;
    void Draw_NO_SHADER_USE(glm::mat4 PROJECTION, glm::mat4 VIEW_MATRIX,
                            glm::vec3 VIEW_POS, float ScaleFactor,
                            Shader &CUSTOM_SHADER) const;
    Shader *ModelShader;
    void SetID(int ID);

    glm::vec3 color;
    unsigned int ID;
    Shader *SELECTION_SHADER;
    bool isSelected;

  private:
    // Basics
    std::string NAME;

    // Model Data
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;

    void LoadModel(std::string path);
    void ProcessNode(aiNode *node, const aiScene *scene);
    Mesh ProcessMesh(aiMesh *mesh, const aiScene *scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial *mat,
                                              aiTextureType type,
                                              std::string typeName);
    // Selection Code

    unsigned int r;
};
