#ifndef BREEZE_MESH_H
#define BREEZE_MESH_H

#include <vector>
#include <glm/glm.hpp>
#include "BREEZE/texture.h"
#include "BREEZE/shader_program.h"

namespace BREEZE
{
struct vertex_t
{
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 tex_coord;
};

class Mesh {
private:
  std::vector<vertex_t> vertices;
  std::vector<uint32_t> indices;
  std::vector<Texture*> textures;

  uint32_t VAO, VBO, EBO;

public:
  Mesh(const std::vector<vertex_t>& vertices, const std::vector<uint32_t>& indices, const std::vector<Texture*>& textures);
  void render(ShaderProgram* shader);
};
}  // namespace BREEZE
#endif  // BREEZE_MESH_H
