#include "BREEZE/shader.h"
#include <string>
#include <iostream>
#include <fstream>
#include <iterator>
#include <GL/glew.h>

namespace BREEZE
{
Shader::Shader(const char* filename, enum shader_type shader_type) : filename(filename), type(shader_type) {
}
// Lê o conteúdo do arquivo shader
std::string Shader::read_file(const std::string& filename) {
  std::string   contents;
  std::ifstream shader_file(filename);

  if (shader_file.is_open()) {
    contents = std::string(std::istreambuf_iterator<char>(shader_file), std::istreambuf_iterator<char>());
  } else {
    std::cerr << "Could not open file: " << filename << std::endl;
  }

  return contents;
}
// Compila o shader
bool Shader::compile() {

  std::string contents = read_file(this->filename);
  GLenum      gl_shader_type;
  // Define o tipo de shader OpenGL com base no tipo especificado
  switch (type) {
    case vertex:
      gl_shader_type = GL_VERTEX_SHADER;
      break;
    case fragment:
      gl_shader_type = GL_FRAGMENT_SHADER;
      break;
  }
  // Cria o shader no OpenGL
  this->shader_id           = glCreateShader(gl_shader_type);
  const char* source_string = contents.c_str();
  glShaderSource(this->shader_id, 1, &source_string, nullptr);
  glCompileShader(this->shader_id);

  char log[512];
  int  success;
  glGetShaderiv(this->shader_id, GL_COMPILE_STATUS, &success);
  // Verifica se houve erro na compilação
  if (!success) {
    glGetShaderInfoLog(this->shader_id, 512, nullptr, log);
    std::cerr << this->filename << ": Shader compilation failed -> " << log << std::endl;
    return false;
  }

  return true;
}
// Destrutor da classe Shader
Shader::~Shader() {
  glDeleteShader(this->shader_id);
}
// Obtém o ID do shader
uint32_t Shader::get_shader_id() const {
  return this->shader_id;
}
}  // namespace BREEZE
