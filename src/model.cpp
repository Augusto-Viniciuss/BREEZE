#include "BREEZE/model.h"
#include "BREEZE/shader_program.h"

#include <iostream>
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

namespace BREEZE
{
Model::Model(const char* filename, Texture* texture_diffuse, Texture* texture_specular) {
  Assimp::Importer importer;

  model                = glm::mat4(1.0f);
  const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    std::cerr << "Failed to load: " << filename << ", " << importer.GetErrorString() << std::endl;
  }

  this->texture_diffuse  = texture_diffuse;
  this->texture_specular = texture_specular;

  processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    meshes.push_back(processMesh(mesh, scene));
  }

  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene);
  }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
  std::vector<vertex_t> vertices;
  std::vector<uint32_t> indices;
  std::vector<Texture*> textures;

  for (int i = 0; i < mesh->mNumVertices; i++) {
    vertex_t v;
    v.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
    v.normal   = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
    if (mesh->mTextureCoords[0]) {
      v.tex_coord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
    } else {
      v.tex_coord = glm::vec2(0.0f, 0.0f);
    }

    vertices.push_back(v);
  }

  for (int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];
    for (uint32_t j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }

  if (mesh->mMaterialIndex >= 0) {
    /* aiMaterial*           material    = scene->mMaterials[mesh->mMaterialIndex]; */
    /* std::vector<Texture*> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse"); */
    /* textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end()); */
    /* std::vector<Texture*> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular"); */
    /* textures.insert(textures.end(), specularMaps.begin(), specularMaps.end()); */
  }

  return Mesh(vertices, indices, textures);
}

std::vector<Texture*> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
  std::vector<Texture*> textures;
  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
    if (typeName == "texture_diffuse" && texture_diffuse != nullptr) {
      textures.push_back(texture_diffuse);
    } else if (typeName == "texture_specular" && texture_specular != nullptr) {
      textures.push_back(texture_specular);
    }
  }

  return textures;
}

void Model::render(ShaderProgram* shader) {
  int model_location = glGetUniformLocation(shader->get_id(), "model");
  glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
  glActiveTexture(GL_TEXTURE0);
  texture_diffuse->use();
  for (auto& m : meshes)
    m.render(shader);
}

void Model::render() {
  if (this->shader_program == nullptr)
    throw std::runtime_error("No shader associated with model!");
  render(this->shader_program);
}

void Model::setShader(ShaderProgram* shader) {
  this->shader_program = shader;
}

ShaderProgram* Model::getShader() const {
  return shader_program;
}

void Model::setTexture(Texture* texture, std::string typeName) {
  if (typeName == "texture_diffuse") {
    this->texture_diffuse = texture;
  } else if (typeName == "texture_specular") {
    this->texture_specular = texture;
  }
}
}  // namespace BREEZE
