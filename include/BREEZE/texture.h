#ifndef BREEZE_TEXTURE_H
#define BREEZE_TEXTURE_H

#include <stdint.h>

namespace BREEZE
{
class Texture {
private:
  uint32_t texture_id;

public:
  explicit Texture(const char* filename);
  bool     load(const char* filename);
  void     use();
  uint32_t get_id() const;
  ~Texture();
};
}  // namespace BREEZE
#endif  // BREEZE_TEXTURE_H
