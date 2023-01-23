module;
#ifdef __APPLE__
#include <new>
#endif

export module yoyo:writer;
import traits;

using namespace traits::ints;

namespace yoyo {
export class writer {
public:
  // TODO: Error handling (aka "follow what was done in the reader")
  virtual void write(const void *buffer, unsigned len) = 0;
  virtual void write_u8(uint8_t data) = 0;
  virtual void write_u32(uint32_t data) = 0;
  virtual void seekp(unsigned pos) = 0;
  virtual unsigned tellp() = 0;
};
} // namespace yoyo
