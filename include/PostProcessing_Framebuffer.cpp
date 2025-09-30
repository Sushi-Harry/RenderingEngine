#include "PostProcessing_Framebuffer.hpp"
#include "ShaderClass.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"

float vertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
};

POSTPROCESSING_BUFFER::POSTPROCESSING_BUFFER(int WINDOW_WIDTH, int WINDOW_HEIGHT, std::string NAME, std::string PATH_TO_FRAGMENT_SHADER){

    // Shader
    SHADER = new Shader("include/Essentials/PostprocessingShaders/screenQuad.vs", PATH_TO_FRAGMENT_SHADER.c_str());
    ENABLED = true;
    this->NAME = NAME;

    // Screen Quad
    SetupScreenQuad();

    // Generating the framebuffer
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    // Generating the texture for the framebuffer
    glGenTextures(1, &TEXTURE);
    glBindTexture(GL_TEXTURE_2D, TEXTURE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Attach the render texture to the framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TEXTURE, 0);
    
    //Generating the data for the RBO
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WINDOW_WIDTH, WINDOW_HEIGHT);
    // Attaching the renderbuffer to the framebuffer we just created
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
    
    // CHecking if the framebuffer is complete
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
        printf("POSTPROCESSING_FRAMEBUFFER_COMPLETE \n");
    
    // Unbind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void POSTPROCESSING_BUFFER::PostProcessing_START(){
    
    // 1st Render Pass: This code Executes before drawing the main scene
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    // Since we're not using the stencil testing stuff here, just use the depth buffer bit and color buffer bit in the glClear function
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    // DRAW THE SCENE AFTER CALLING THIS FUNCTION
}

void POSTPROCESSING_BUFFER::PostProcessing_END(){
    // Binding to the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    SHADER->use();
    glBindVertexArray(VAO);
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, TEXTURE);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void POSTPROCESSING_BUFFER::SetupScreenQuad(){
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // This is the Position Attrib
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    // This is the texCoord attrib
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void POSTPROCESSING_BUFFER::toggleEffect(){
    ENABLED = !ENABLED;
}

void POSTPROCESSING_BUFFER::takeInput(){
    std::string label = NAME + " Toggle";
    ImGui::Checkbox(label.c_str(), &ENABLED);
}

POSTPROCESSING_BUFFER::~POSTPROCESSING_BUFFER(){
    
}