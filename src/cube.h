#pragma once

#include "linalg.h"
#include "gpuProgram.h"
#include "seq.h"

class Cube
{
public:
    Cube();


    GPUProgram        gpu;

    static const char* vs;
    static const char* fs;
    void Draw(mat4& MV, mat4& MVP, vec3 lightDir, vec3 color);
private:
    void SetUpVAO();
    unsigned int VAO;
};