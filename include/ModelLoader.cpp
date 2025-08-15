#include "ModelLoader.hpp"
#include "Mesh.hpp"
#include "ShaderClass.hpp"
#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/types.h>
#include <glm/ext/matrix_transform.hpp>
#include <string>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Helper Function to load and reture the id of a texture
unsigned int TextureFromFile(const char *path, const std::string &directory){
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    std::cout << "Loading texture from: " << filename << std::endl;  // --------- DEBUG -------------

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

Model::Model(const std::string &PATH, const std::string &SHADER_PATH_VS, const std::string &SHADER_PATH_FS, std::string NAME){
    LoadModel(PATH);
    this->NAME = NAME;
    ModelShader = new Shader(SHADER_PATH_VS.c_str(), SHADER_PATH_FS.c_str());
    SELECTION_SHADER = new Shader("Essentials/ImpShaders/selectionMode.vs", "Essentials/ImpShaders/selectionMode.fs");
}

void Model::SetID(int ID){
    this->ID = ID;
}

void Model::Draw(glm::mat4 PROJECTION, glm::mat4 VIEW_MATRIX, glm::vec3 VIEW_POS, float ScaleFactor){
    for(unsigned int i = 0; i < meshes.size(); i++){
        ModelShader->use();
        ModelShader->setFloat("material.shininess", 32.0f);
        glm::mat4 model = glm::mat4(1.0f);
        scaleModel(ScaleFactor, model);
        ModelShader->setMat4("projection", PROJECTION);
        ModelShader->setMat4("view", VIEW_MATRIX);
        ModelShader->setVec3("viewPos", VIEW_POS);
        ModelShader->setMat4("model", model);
        
        meshes[i].Draw(*ModelShader);
    }
}

void Model::Draw_NO_SHADER_USE(glm::mat4 PROJECTION, glm::mat4 VIEW_MATRIX, glm::vec3 VIEW_POS, float ScaleFactor, Shader& CUSTOM_SHADER){
    for(unsigned int i = 0; i < meshes.size(); i++){
        CUSTOM_SHADER.use();
        CUSTOM_SHADER.setFloat("material.shininess", 32.0f);
        glm::mat4 model = glm::mat4(1.0f);
        scaleModel(ScaleFactor, model);
        CUSTOM_SHADER.setMat4("projection", PROJECTION);
        CUSTOM_SHADER.setMat4("view", VIEW_MATRIX);
        CUSTOM_SHADER.setVec3("viewPos", VIEW_POS);
        CUSTOM_SHADER.setMat4("model", model);
        
        meshes[i].Draw(*ModelShader);
    }
}

void Model::Draw_SELECTIONMODE(glm::mat4 PROJECTION, glm::mat4 VIEW_MATRIX, glm::vec3 VIEW_POS, float ScaleFactor){
    for(unsigned int i = 0; i < meshes.size(); i++){
        SELECTION_SHADER->use();
        SELECTION_SHADER->setFloat("material.shininess", 32.0f);
        glm::mat4 model = glm::mat4(1.0f);

        scaleModel(ScaleFactor, model);
        model = glm::translate(model, glm::vec3(0.0f));

        SELECTION_SHADER->setMat4("projection", PROJECTION);
        SELECTION_SHADER->setMat4("view", VIEW_MATRIX);
        SELECTION_SHADER->setVec3("viewPos", VIEW_POS);
        SELECTION_SHADER->setMat4("model", model);
        
        meshes[i].Draw(*SELECTION_SHADER);
    }
}

void Model::LoadModel(std::string PATH){
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(PATH, aiProcess_Triangulate | aiProcess_FlipUVs);
    
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    directory = PATH.substr(0, PATH.find_last_of('/'));
    std::cout << "MODEL DIRECTORY RESOLVED AS: " << directory << std::endl;
    ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode* NODE, const aiScene* SCENE){
    // Processing all the nodes' meshes (if there are any)
    for(unsigned int i = 0; i < NODE->mNumMeshes; i++){
        aiMesh* mesh = SCENE->mMeshes[NODE->mMeshes[i]];
        meshes.push_back(ProcessMesh(mesh, SCENE));
    }

    for(unsigned int i = 0; i < NODE->mNumChildren; i++){
        ProcessNode(NODE->mChildren[i], SCENE);
    }
}

Mesh Model::ProcessMesh(aiMesh* MESH, const aiScene* SCENE){
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for(unsigned int i = 0; i < MESH->mNumVertices; i++){
        Vertex vertex;
        // processing the vertices
        glm::vec3 posVector, normalVector;

        posVector.x = MESH->mVertices[i].x;
        posVector.y = MESH->mVertices[i].y;
        posVector.z = MESH->mVertices[i].z;
        vertex.position = posVector;

        normalVector.x = MESH->mNormals[i].x;
        normalVector.y = MESH->mNormals[i].y;
        normalVector.z = MESH->mNormals[i].z;
        vertex.normal = normalVector;

        if(MESH->mTextureCoords[0]){
            glm::vec2 texVec;
            texVec.x = MESH->mTextureCoords[0][i].x;
            texVec.y = MESH->mTextureCoords[0][i].y;

            vertex.textureCoords = texVec;
        }else{
            vertex.textureCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }
    
    // Processing the indices ==== number of faces = number of indices
    for(unsigned int i = 0; i < MESH->mNumFaces; i++){
        aiFace face = MESH->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++){
            indices.push_back(face.mIndices[j]);
        }
    }

    if(MESH->mMaterialIndex >= 0){
        // Processing material
        aiMaterial* material = SCENE->mMaterials[MESH->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }
    
    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        std::string texturePath = str.C_Str();
        std::cout << "Trying to load texture: " << texturePath << std::endl;

        // Check if texture was loaded before
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++) {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }

        if (!skip) { // Not loaded before
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);  // Add to cache
        }
    }

    return textures;
}

void Model::scaleModel(float ScaleFactor, glm::mat4& model_matrix){
    model_matrix = glm::scale(model_matrix, glm::vec3(ScaleFactor));
}