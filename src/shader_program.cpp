#include "BREEZE/shader_program.h"
#include <GL/glew.h>
#include <iostream>

namespace BREEZE
{
  //Recebe dois shaders (vertex e fragment) e os liga em um programa de shader
ShaderProgram::ShaderProgram(Shader* vShader, Shader* fShader) {
  this->id = glCreateProgram();
  glAttachShader(this->id, vShader->get_shader_id());
  glAttachShader(this->id, fShader->get_shader_id());
  glLinkProgram(this->id);

  int success;
  glGetProgramiv(this->id, GL_LINK_STATUS, &success);
  if (!success) {
    char log[512];
    glGetProgramInfoLog(this->id, 512, nullptr, log);
    std::cerr << "Failed to link shader program: " << std::endl;
  }
}
// Função que retorna o identificador (ID) do programa de shader
uint32_t ShaderProgram::get_id() const {
  return this->id;
}
// Função que ativa o programa de shader (torna-o o shader atual que será usado pela pipeline do OpenGL)
void ShaderProgram::use() {
  glUseProgram(this->id);
}
// Destrutor da classe ShaderProgram 
ShaderProgram::~ShaderProgram() {
  glDeleteProgram(this->id);
}
}  // namespace BREEZE
