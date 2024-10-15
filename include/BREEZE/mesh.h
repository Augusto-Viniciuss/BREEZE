#ifndef BREEZE_MESH_H
#define BREEZE_MESH_H

#include <vector>
#include <glm/glm.hpp>
#include "BREEZE/texture.h"
#include "BREEZE/shader_program.h"

namespace BREEZE
{

// Estrutura que representa um vértice
// Cada vértice tem uma posição (3D), uma normal (3D) e coordenadas de textura (2D)
struct vertex_t
{
  glm::vec3 position;  // Posição do vértice no espaço 3D
  glm::vec3 normal;    // Normal do vértice para cálculos de iluminação
  glm::vec2 tex_coord; // Coordenadas de textura para mapeamento de textura
};

// Classe que representa uma malha (Mesh)
// Uma malha é composta de vértices, índices e texturas, e pode ser renderizada
class Mesh {
private:
  std::vector<vertex_t> vertices;     // Vetor de vértices que compõem a malha
  std::vector<uint32_t> indices;      // Vetor de índices que define como os vértices formam triângulos
  std::vector<Texture*> textures;     // Vetor de ponteiros para texturas associadas à malha

  uint32_t VAO, VBO, EBO;             // Identificadores de OpenGL para o Vertex Array Object (VAO),
                                      // Vertex Buffer Object (VBO) e Element Buffer Object (EBO)

public:
  // Inicializa a malha com os vértices, índices e texturas fornecidos
  Mesh(const std::vector<vertex_t>& vertices, const std::vector<uint32_t>& indices, const std::vector<Texture*>& textures);

  // Método que renderiza a malha utilizando um ShaderProgram fornecido
  void render(ShaderProgram* shader);
};
}  // namespace BREEZE

#endif  // BREEZE_MESH_H
