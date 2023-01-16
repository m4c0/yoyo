module;
#include <stdint.h>

export module yoyo:writer;

namespace yoyo {
export class writer {
public:
  // TODO: Error handling
  virtual void write(const void *buffer, unsigned len) = 0;
  virtual void write_u8(uint8_t data) = 0;
  virtual void write_u32(uint32_t data) = 0;
  virtual void seekp(unsigned pos) = 0;
  virtual unsigned tellp() = 0;
};
} // namespace yoyo
