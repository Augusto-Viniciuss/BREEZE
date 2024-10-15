#ifndef BREEZE_SHADER_PROGRAM_H
#define BREEZE_SHADER_PROGRAM_H

#include "BREEZE/shader.h"
#include <stdint.h>

namespace BREEZE
{
// Um programa de shader é composto por dois shaders principais: vertex shader e fragment shader
class ShaderProgram {
private:
  uint32_t id; // ID do programa de shaders no OpenGL

public:
  // Construtor que recebe dois shaders (vertex e fragment) e os liga para formar um programa de shader
  explicit ShaderProgram(Shader* vShader, Shader* fShader);

  // Retorna o ID do programa de shaders, que é gerado pelo OpenGL
  uint32_t get_id() const;

  // Ativa o programa de shaders para ser usado na renderização
  void use();

  // Destrutor que libera os recursos associados ao programa de shaders
  ~ShaderProgram();
};
}  // namespace BREEZE
#endif  // BREEZE_SHADER_PROGRAM_H
