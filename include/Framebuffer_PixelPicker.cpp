#include "Framebuffer_PixelPicker.hpp"
#include "Engine.hpp"
#include "ModelLoader.hpp"
#include "ShaderClass.hpp"
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

SELECTION_FRAMEBUFFER::SELECTION_FRAMEBUFFER(int WINDOW_WIDTH, int WINDOW_HEIGHT) {
    FLAT_SHADER = new Shader("include/Essentials/ImpShaders/selectionMode.vs", "include/Essentials/ImpShaders/selectionMode.fs");

    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // Generating the texture to assign to the framebuffer
    glGenTextures(1, &TEXTURE);
    glBindTexture(GL_TEXTURE_2D, TEXTURE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TEXTURE, 0);

    // Generating the data for the renderbuffer
    glGenRenderbuffers(1, &DEPTH_RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, DEPTH_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, DEPTH_RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR::SELECTION_FRAMEBUFFER_NOT_COMPLETE" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::vec3 SELECTION_FRAMEBUFFER::OBJECT_ID_TO_COLOR(unsigned int ID) {
    // This is a standard and reliable way to encode an ID into an RGB color.
    int r = (ID & 0x000000FF) >> 0;
    int g = (ID & 0x0000FF00) >> 8;
    int b = (ID & 0x00FF0000) >> 16;
    return glm::vec3(r / 255.0f, g / 255.0f, b / 255.0f);
}

unsigned int SELECTION_FRAMEBUFFER::COLOR_TO_OBJECT_ID(unsigned char color[3]) {
    return color[0] + color[1] * 256 + color[2] * 256 * 256;
}

void SELECTION_FRAMEBUFFER::RENDER_TO_FBO(glm::mat4 PROJECTION, glm::mat4 VIEW_MATRIX,
                                          glm::vec3 VIEW_POS, int WINDOW_WIDTH, int WINDOW_HEIGHT,
                                          // --- FIX 2: Pass vector by const reference ---
                                          const std::vector<Model>& MODEL_ARRAY) {
    // --- FIX 3: Bind to GL_FRAMEBUFFER, not GL_RENDERBUFFER ---
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    // Clear with ID 0, which we can reserve for "no object" / background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST); // Depth testing is important for picking too!

    FLAT_SHADER->use();
    for (const auto& model : MODEL_ARRAY) {
        glm::vec3 COLOR = OBJECT_ID_TO_COLOR(model.ID);
        FLAT_SHADER->setVec3("PickingColor", COLOR);
        model.Draw_NO_SHADER_USE(PROJECTION, VIEW_MATRIX, VIEW_POS, 1.0f, *FLAT_SHADER);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int SELECTION_FRAMEBUFFER::READ_PICKED_ID(GLFWwindow* WINDOW, int WINDOW_WIDTH, int WINDOW_HEIGHT) {
    double xPos, yPos;
    glfwGetCursorPos(WINDOW, &xPos, &yPos);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, FBO);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    // --- FIX 4: Read a 1x1 pixel, not the whole screen ---
    unsigned char PIXELS[3];
    // --- FIX 5: Invert Y coordinate for OpenGL ---
    glReadPixels((int)xPos, WINDOW_HEIGHT - (int)yPos, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &PIXELS);

    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    
    return COLOR_TO_OBJECT_ID(PIXELS);
}

SELECTION_FRAMEBUFFER::~SELECTION_FRAMEBUFFER() {
    glDeleteFramebuffers(1, &FBO);
    glDeleteTextures(1, &TEXTURE);
    glDeleteRenderbuffers(1, &DEPTH_RBO);
    delete FLAT_SHADER;
}
