#ifndef BREEZE_SHADER_H
#define BREEZE_SHADER_H

#include <string>
#include <stdint.h>

namespace BREEZE
{
enum shader_type
{
  vertex,
  fragment,
};

class Shader {
private:
  std::string filename;
  shader_type type;
  uint32_t    shader_id;

  static std::string read_file(const std::string& filename);

public:
  explicit Shader(const char* filename, enum shader_type shader_type);
  bool     compile();
  uint32_t get_shader_id() const;
  ~Shader();
};
}  // namespace BREEZE
#endif  // BREEZE_SHADER_H
