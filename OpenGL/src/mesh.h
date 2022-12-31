#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "texture.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};


class Mesh {
public:
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<texture>      textures;

    aiColor4D specular, diffuse;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<texture> textures);
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, const aiColor4D& diffuse, const aiColor4D& specular);
    void Render();
private:
    //  render data
    unsigned int VAO, VBO, EBO;
    bool no_textures;

    void SetupMesh();
};