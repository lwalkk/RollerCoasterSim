#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "seq.h"
#include "linalg.h"

class ObjModel
{
    ObjModel(const char*);

    seq<vec3> verts;
    seq<vec3> norms;
    seq<vec3> faces;
    seq<vec2> uv;
private:
    const char* filepath;
};
