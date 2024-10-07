#ifndef BREEZE_MODEL_H
#define BREEZE_MODEL_H

#include <vector>
#include "BREEZE/mesh.h"
#include "BREEZE/texture.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace BREEZE
{
class Model {
public:
  explicit Model(const char* filename, Texture* texture_diffuse, Texture* texture_specular);
  void           render(ShaderProgram* shader_program);
  void           render();
  void           setShader(ShaderProgram* shader_program);
  void           setTexture(Texture* texture, std::string typeName);
  ShaderProgram* getShader() const;

  glm::mat4 model;

private:
  std::vector<Mesh> meshes;
  Texture*          texture_diffuse;
  Texture*          texture_specular;

  ShaderProgram*        shader_program = nullptr;
  std::vector<Texture*> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
  void                  processNode(aiNode* node, const aiScene* scene);
  Mesh                  processMesh(aiMesh* mesh, const aiScene* scene);
};
}  // namespace BREEZE
#endif  // BREEZE_MODEL_H
