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
// Classe que representa um modelo 3D, composto por múltiplas malhas
// Cada modelo pode ser carregado a partir de um arquivo usando a biblioteca Assimp para processar os dados
class Model {
public:
  // Construtor que carrega o modelo de um arquivo e aplica texturas difusa e especular
  explicit Model(const char* filename, Texture* texture_diffuse, Texture* texture_specular);

  // Renderiza o modelo utilizando o ShaderProgram fornecido
  void render(ShaderProgram* shader_program);
  void render();
  // Define o ShaderProgram a ser utilizado durante a renderização
  void setShader(ShaderProgram* shader_program);
  // Define uma textura específica (difusa ou especular) com base no nome do tipo
  void setTexture(Texture* texture, std::string typeName);
  // Retorna o ShaderProgram atualmente associado ao modelo
  ShaderProgram* getShader() const;
  // Matriz de transformação do modelo, usada para posicionamento, rotação e escala
  glm::mat4 model;

private:
  // Vetor que armazena as malhas do modelo
  std::vector<Mesh> meshes;

  // Ponteiros para texturas difusas e especulares aplicadas ao modelo
  Texture* texture_diffuse;
  Texture* texture_specular;

  // Shader program utilizado para renderizar o modelo
  ShaderProgram* shader_program = nullptr;
  // Função que carrega as texturas de um material utilizando Assimp
  std::vector<Texture*> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

  // Processa os nós da cena carregada pela Assimp
  // Cada nó pode conter malhas ou referenciar outros nós
  void processNode(aiNode* node, const aiScene* scene);
  // Processa uma única malha da cena carregada e converte para o formato interno de Mesh
  Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};

}  // namespace BREEZE

#endif  // BREEZE_MODEL_H
