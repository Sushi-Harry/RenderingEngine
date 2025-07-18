#!/bin/bash

echo "Name of the Lit Shader: (without extension)"
read -r FILE_NAME

EXTENSION_VS=".vs"
EXTENSION_FS=".fs"
FILENAME_VS="${FILE_NAME}${EXTENSION_VS}" 
FILENAME_FS="${FILE_NAME}${EXTENSION_FS}"

cd $HOME/ProjectsDirectory/OpenGL_Projects/RenderingEngine/

cp BASE_SHADERS/LIT_BASE_SHADER.vs shaders/
cp BASE_SHADERS/LIT_BASE_SHADER.fs shaders/

mv shaders/LIT_BASE_SHADER.vs shaders/"$FILENAME_VS"
mv shaders/LIT_BASE_SHADER.fs shaders/"$FILENAME_FS"