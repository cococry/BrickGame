#pragma once

#include "mesh.h"

#include <vector>
#include <assimp/scene.h>

class Model
{
public:
    Model() = default;
    Model(const std::string& path, bool noTex = false)
    {
        no_textures = noTex;
        loadModel(path);
    }
    void Render();
private:
    // model data
    std::vector<Mesh> meshes;
    std::string directory;

    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type);

    std::vector<texture> textures_loaded;
    bool no_textures = false;
};