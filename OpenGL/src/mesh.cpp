#include "mesh.h"
#include "render_state.h"

#include <glad/glad.h>
#include <iostream>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    no_textures = false;

    SetupMesh();
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, const aiColor4D& diffuse, const aiColor4D& specular) 
    : vertices(vertices), indices(indices), diffuse(diffuse), specular(specular), no_textures(true)
{
    SetupMesh();
}

void Mesh::Render()
{
    if (no_textures) {
        RenderState::GetShader()->UploadVec4("diffuse", glm::vec4(diffuse.r, diffuse.g, diffuse.b, diffuse.a));
        RenderState::GetShader()->UploadVec4("specular", glm::vec4(specular.r, specular.g, specular.b, specular.a));
        RenderState::GetShader()->UploadInt("no_textures", 1);
    }
    else {
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            RenderState::GetShader()->UploadInt("no_textures", 0);
            glActiveTexture(GL_TEXTURE0 + i);
            std::string number;
            std::string name;
            switch (textures[i].Type) {
            case aiTextureType_DIFFUSE:
                name = "diffuse" + std::to_string(diffuseNr++);
                break;
            case aiTextureType_SPECULAR:
                name = "specular" + std::to_string(specularNr++);
                break;
            }
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++);

            std::cout << name << "\n";
            RenderState::GetShader()->UploadInt(name, i);
            glBindTextureUnit(GL_TEXTURE0 + i, textures[i].get_id());
            textures[i].bind();

        }
    }
    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    if (!no_textures) {
        glActiveTexture(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Mesh::SetupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
        &indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}
