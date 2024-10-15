#ifndef BREEZE_SHADER_H
#define BREEZE_SHADER_H

#include <string>
#include <stdint.h>

namespace BREEZE
{

// Enumeração para definir o tipo de shader (vertex ou fragment)
enum shader_type
{
  vertex,   // Shader de vértice (Vertex Shader)
  fragment, // Shader de fragmento (Fragment Shader)
};

// Cada shader é responsável por uma etapa do pipeline de renderização gráfica (vértice ou fragmento)
class Shader {
private:
  std::string filename;   // Nome do arquivo onde o código-fonte do shader está armazenado
  shader_type type;       // Tipo do shader (vertex ou fragment)
  uint32_t    shader_id;  // ID do shader no contexto do OpenGL

  // Função estática que lê o conteúdo de um arquivo e retorna como string
  static std::string read_file(const std::string& filename);

public:
  // Construtor que recebe o nome do arquivo do shader e o tipo (vertex ou fragment)
  explicit Shader(const char* filename, enum shader_type shader_type);

  // Compila o código-fonte do shader e retorna se a compilação foi bem-sucedida ou não
  bool compile();

  // Retorna o ID do shader
  uint32_t get_shader_id() const;

  // Destrutor que libera os recursos do shader 
  ~Shader();
};

}  // namespace BREEZE
#endif  // BREEZE_SHADER_H
