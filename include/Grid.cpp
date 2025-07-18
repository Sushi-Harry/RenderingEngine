#include "Grid.hpp"
#include "ShaderClass.hpp"
#include <glm/ext/matrix_transform.hpp>

float quadVertices[] = {
    // positions          // texCoords
    -1.0f, 0.0f, -1.0f,   0.0f, 0.0f,
     1.0f, 0.0f, -1.0f,   1.0f, 0.0f,
     1.0f, 0.0f,  1.0f,   1.0f, 1.0f,
    -1.0f, 0.0f,  1.0f,   0.0f, 1.0f,
};

unsigned int indices[] = {
    0, 1, 2,
    2, 3, 0
};

unsigned int indexLength = sizeof(indices) / sizeof(indices[0]);

Grid::Grid(){
    GridShader = new Shader("shaders/grid.vs", "shaders/grid.fs");
    setupGrid();
}

void Grid::setupGrid(){
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);
    glBindVertexArray(0);
}

void Grid::DrawGrid(glm::mat4 PROJECTION_MATRIX, glm::mat4 VIEW_MATRIX, glm::vec3 CAMERA_POSITION){
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(glm::mat4(1.0f), glm::vec3(CAMERA_POSITION.x, 0.0f, CAMERA_POSITION.z));
    model = glm::scale(model, glm::vec3(1000.0f));

    GridShader->use();
    GridShader->setMat4("projection", PROJECTION_MATRIX);
    GridShader->setMat4("view", VIEW_MATRIX);
    GridShader->setMat4("model", model);
    GridShader->setVec3("CameraPos", CAMERA_POSITION);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexLength, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
}