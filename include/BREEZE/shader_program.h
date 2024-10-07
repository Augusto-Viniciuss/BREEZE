#ifndef BREEZE_SHADER_PROGRAM_H
#define BREEZE_SHADER_PROGRAM_H

#include "BREEZE/shader.h"
#include <stdint.h>

namespace BREEZE
{
class ShaderProgram {
private:
  uint32_t id;

public:
  explicit ShaderProgram(Shader* vShader, Shader* fShader);
  uint32_t get_id() const;
  void     use();
  ~ShaderProgram();
};
}  // namespace BREEZE
#endif  // BREEZE_SHADER_PROGRAM_H
