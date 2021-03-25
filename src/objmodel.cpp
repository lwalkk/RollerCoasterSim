#include "objmodel.h"

ObjModel::ObjModel(const char* filepath)
{
    std::ifstream ifs(filepath, std::ifstream::in);
    std::string line;

    while (std::getline(ifs, line))
    {
        int strpos = line.find(" ");
        std::string dtype = line.substr(0, strpos);
        std::string vals = line.substr(strpos);
        std::stringstream ss(line);

        if (dtype == "v")
        {
            vec3 tempVert;
            ss >> tempVert.x;
            ss >> tempVert.y;
            ss >> tempVert.z;

        }

        if (dtype == "vn")
        {
            vec3 tempNorm;
            ss >> tempNorm.x;
            ss >> tempNorm.y;
            ss >> tempNorm.z;
        }

        if (dtype == "vt")
        {
            vec2 tempUV;
            ss >> tempUV.x;
            ss >> tempUV.y;
        }

        if (dtype == "f")
        {
            std::string vert1, vert2, vert3;
            unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];

        }
    }



}
