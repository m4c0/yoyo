export module yoyo:common;
import traits;

namespace yoyo {
using namespace traits::ints;

export enum class seek_mode { set, current, end };

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

[[nodiscard]] static constexpr uint64_t flip64(uint64_t u64) noexcept {
  constexpr const auto u32_bitsize = 32U;
  uint32_t ab = flip32(static_cast<uint32_t>(u64));
  uint32_t cd = flip32(static_cast<uint32_t>(u64 >> u32_bitsize));
  return (static_cast<uint64_t>(ab) << u32_bitsize) | cd;
}
} // namespace details
} // namespace yoyo
