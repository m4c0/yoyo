module;
#ifdef __APPLE__
#include <new>
#endif

export module yoyo:writer;
import :common;
import jute;
import missingno;
import traits;

namespace yoyo {
export class writer {
public:
  [[nodiscard]] virtual bool ready() const noexcept { return true; }

  [[nodiscard]] virtual mno::req<void> seekp(int pos,
                                             seek_mode mode) noexcept = 0;
  [[nodiscard]] virtual mno::req<unsigned> tellp() const noexcept = 0;

  [[nodiscard]] constexpr mno::req<void> seekp(unsigned pos) noexcept {
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

  [[nodiscard]] constexpr mno::req<void> write(jute::view v) noexcept {
    return write(v.data(), v.size());
  }
  template <unsigned N>
  [[nodiscard]] constexpr mno::req<void> write(jute::twine<N> t) noexcept {
    auto v = t.cstr();
    return write(v.data(), v.size());
  }

  [[nodiscard]] virtual mno::req<void> write_u8(uint8_t n) noexcept {
    return write<uint8_t>(n);
  }
  [[nodiscard]] virtual mno::req<void> write_u16(uint16_t n) noexcept {
    return write<uint16_t>(n);
  }
  [[nodiscard]] virtual mno::req<void> write_u32(uint32_t n) noexcept {
    return write<uint32_t>(n);
  }

  [[nodiscard]] constexpr mno::req<void> write_u16_be(uint16_t n) noexcept {
    return write_u16(details::flip16(n));
  }
  [[nodiscard]] constexpr mno::req<void> write_u32_be(uint32_t n) noexcept {
    return write_u32(details::flip32(n));
  }
};
} // namespace yoyo
