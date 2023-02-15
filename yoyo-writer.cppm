module;
#ifdef __APPLE__
#include <new>
#endif

export module yoyo:writer;
import traits;
import missingno;

using namespace traits::ints;

namespace yoyo {
export enum class seek_mode { set, current, end };
export class writer {
public:
  [[nodiscard]] virtual bool ready() const noexcept { return true; }

  [[nodiscard]] virtual mno::req<void> seekp(int pos,
                                             seek_mode mode) noexcept = 0;
  [[nodiscard]] virtual mno::req<unsigned> tellp() const noexcept = 0;

  [[nodiscard]] constexpr mno::req<void> seekg(unsigned pos) noexcept {
    return seekp(static_cast<int>(pos), seek_mode::set);
  }

  [[nodiscard]] virtual mno::req<void> write(const void *buffer,
                                             unsigned len) noexcept = 0;
  [[nodiscard]] virtual mno::req<void> write(const uint8_t *buffer,
                                             unsigned len) noexcept {
    return write(static_cast<const void *>(buffer), len);
  }

  template <typename T>
  [[nodiscard]] inline mno::req<void> write(T t) noexcept {
    return write(&t, sizeof(T));
  }
};
} // namespace yoyo
