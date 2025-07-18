// #include "TextureLoader.hpp"
// #include "ShaderClass.hpp"
// #include <iostream>

// #define STB_IMAGE_IMPLEMENTATION
// #include "stb_image.h"

// TextureLoader::TextureLoader(std::string DIRECTORY, aiString FILE_NAME){
//     directory = DIRECTORY; // Directory has to be of the form "Blah/blah/"
//     file_name = FILE_NAME.C_Str(); // File Name has to be of type "file_name.png"


//     SetupTexture();
// }

// void TextureLoader::SetupTexture(){
//     glGenTextures(1, &texture);
//     glBindTexture(GL_TEXTURE_2D, texture);

//     // Texture Wrapping and Filtering Options
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//     // Load and Generate the Texture Data
//     std::string path = directory + file_name;
//     std::cout << path.c_str() << std::endl;
//     unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
//     if(data){
//         GLenum format = GL_RGB;
//         if (nrChannels == 1)
//             format = GL_RED;
//         else if (nrChannels == 3)
//             format = GL_RGB;
//         else if (nrChannels == 4)
//             format = GL_RGBA;
//         glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//         glGenerateMipmap(GL_TEXTURE_2D);
//     }else{
//         std::cout << "ERROR::TEXTURE_LOADER::FAILED_TO_LOAD_TEXTURE" << std::endl;
//     }
//     stbi_image_free(data);
// }

// unsigned int TextureLoader::ReturnTextureID(){
//     return texture;
// }