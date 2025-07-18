#include "ShaderClass.hpp"

class Grid{

public:
    Grid();
    void DrawGrid(glm::mat4 PROJECTION_MATRIX, glm::mat4 VIEW_MATRIX, glm::vec3 CAMERA_POSITION);
    ~Grid();
private:
    std::string NAME;
    unsigned int VAO, VBO, EBO;
    Shader* GridShader;
    void setupGrid();

};