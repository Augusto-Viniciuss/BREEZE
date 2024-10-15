#ifndef BREEZE_TEXTURE_H
#define BREEZE_TEXTURE_H

#include <stdint.h>

namespace BREEZE
{
// As texturas são usadas para mapear imagens em superfícies 3D durante o processo de renderização
class Texture {
private:
  uint32_t texture_id;  // ID da textura gerado pelo OpenGL

public:
  // Construtor que recebe o caminho de um arquivo de imagem para carregar como textura
  explicit Texture(const char* filename);

  // Carrega uma textura a partir de um arquivo de imagem e armazena no contexto OpenGL
  bool load(const char* filename);

  // Ativa a textura para ser usada no processo de renderização
  void use();

  // Retorna o ID da textura, utilizado para referenciar a textura no OpenGL
  uint32_t get_id() const;

  // Destrutor que libera os recursos alocados para a textura
  ~Texture();
};
}  // namespace BREEZE
#endif  // BREEZE_TEXTURE_H
