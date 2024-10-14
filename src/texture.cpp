#include "BREEZE/texture.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <iostream>

#include <BREEZE/stb_image.h>

namespace BREEZE
{
// Construtor da classe Texture
Texture::Texture(const char* filename) {
  load(filename);
}
// Função para carregar a textura a partir de um arquivo de imagem
bool Texture::load(const char* filename) {
  glGenTextures(1, &this->texture_id);
  glBindTexture(GL_TEXTURE_2D, this->texture_id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  stbi_set_flip_vertically_on_load(true);
  int            width, height, channels;
  unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);
  if (!data) {
    std::cerr << "Failed to open: " << filename << std::endl;
    return false;
  } else {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }

  stbi_image_free(data);
  return true;
}
// Função para ativar a textura e torná-la utilizável no pipeline de OpenGL
void Texture::use() {
  glBindTexture(GL_TEXTURE_2D, this->texture_id);
}
// Função que retorna o ID da textura
uint32_t Texture::get_id() const {
  return this->texture_id;
}
// Destrutor da classe Texture
Texture::~Texture() {
  GLuint textures_to_delete[] = {this->texture_id};
  glDeleteTextures(1, textures_to_delete);
}
}  // namespace BREEZE
