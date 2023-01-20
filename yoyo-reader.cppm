module;
#include <stdint.h>

export module yoyo:reader;
import missingno;

namespace yoyo {
namespace details {
[[nodiscard]] static constexpr uint16_t flip16(uint16_t u16) noexcept {
  constexpr const auto u8_bitsize = 8U;
  uint8_t a = u16;
  uint8_t b = u16 >> u8_bitsize;
  return static_cast<uint16_t>(a << u8_bitsize) | b;
}

[[nodiscard]] static constexpr uint32_t flip32(uint32_t u32) noexcept {
  constexpr const auto u16_bitsize = 16U;
  uint16_t ab = flip16(static_cast<uint16_t>(u32));
  uint16_t cd = flip16(static_cast<uint16_t>(u32 >> u16_bitsize));
  return static_cast<uint32_t>(ab << u16_bitsize) | cd;
}
static constexpr const auto test_be = 0xaabbccddU;
static constexpr const auto test_le = 0xddccbbaaU;
static_assert(flip32(test_be) == test_le);
} // namespace details

export enum class seek_mode { set, current, end };

export class reader {
  friend class subreader;

protected:
  template <typename T> using req = mno::req<T>;

public:
  virtual ~reader() = default;

  [[nodiscard]] virtual bool ready() const noexcept { return true; }

  [[nodiscard]] virtual req<bool> eof() const noexcept = 0;
  [[nodiscard]] virtual req<void> seekg(int pos, seek_mode mode) noexcept = 0;
  [[nodiscard]] virtual req<unsigned> tellg() const noexcept = 0;

  [[nodiscard]] constexpr req<void> seekg(unsigned pos) noexcept {
    return seekg(static_cast<int>(pos), seek_mode::set);
  }

  [[nodiscard]] virtual req<void> read(uint8_t *buffer, unsigned len) noexcept {
    return read(static_cast<void *>(buffer), len);
  }
  [[nodiscard]] virtual req<void> read(void *buffer, unsigned len) noexcept = 0;

  template <typename T> [[nodiscard]] inline req<T> read() noexcept {
    T res;
    return read(&res, sizeof(res)).map([&] { return res; });
  }

  [[nodiscard]] virtual req<uint8_t> read_u8() noexcept {
    return read<uint8_t>();
  }
  [[nodiscard]] virtual req<uint16_t> read_u16() noexcept {
    return read<uint16_t>();
  }
  [[nodiscard]] virtual req<uint32_t> read_u32() noexcept {
    return read<uint32_t>();
  }

  [[nodiscard]] constexpr req<uint16_t> read_u16_be() noexcept {
    return read_u16().map(details::flip16);
  }
  [[nodiscard]] constexpr req<uint32_t> read_u32_be() noexcept {
    return read_u32().map(details::flip32);
  }
};
} // namespace yoyo
