export module yoyo:reader;
import :common;
import missingno;
import traits;

using namespace traits::ints;

namespace yoyo {
export class reader {
  friend class subreader;

protected:
  template <typename T> using req = mno::req<T>;

public:
  virtual ~reader() = default;

  [[nodiscard]] virtual bool ready() const { return true; }

  [[nodiscard]] virtual req<bool> eof() const = 0;
  [[nodiscard]] virtual req<void> seekg(int64_t pos, seek_mode mode) = 0;
  [[nodiscard]] virtual req<uint64_t> tellg() const = 0;

  [[nodiscard]] constexpr req<void> seekg(uint64_t pos) {
    return seekg(static_cast<int64_t>(pos), seek_mode::set);
  }

  [[nodiscard]] virtual req<void> read(uint8_t *buffer, unsigned len) {
    return read(static_cast<void *>(buffer), len);
  }
  [[nodiscard]] virtual req<void> read(void *buffer, unsigned len) = 0;
  [[nodiscard]] virtual req<unsigned> read_up_to(uint8_t *buffer,
                                                 unsigned len) {
    return read_up_to(static_cast<void *>(buffer), len);
  }
  [[nodiscard]] virtual req<unsigned> read_up_to(void *buffer,
                                                 unsigned len) = 0;

  template <typename T> [[nodiscard]] inline req<T> read() {
    T res;
    return read(&res, sizeof(res)).map([&] { return res; });
  }

  [[nodiscard]] virtual req<int8_t> read_s8() { return read<int8_t>(); }
  [[nodiscard]] virtual req<int16_t> read_s16() { return read<int16_t>(); }
  [[nodiscard]] virtual req<int32_t> read_s32() { return read<int32_t>(); }
  [[nodiscard]] virtual req<int64_t> read_s64() { return read<int64_t>(); }

  [[nodiscard]] virtual req<uint8_t> read_u8() { return read<uint8_t>(); }
  [[nodiscard]] virtual req<uint16_t> read_u16() { return read<uint16_t>(); }
  [[nodiscard]] virtual req<uint32_t> read_u32() { return read<uint32_t>(); }
  [[nodiscard]] virtual req<uint64_t> read_u64() { return read<uint64_t>(); }

  [[nodiscard]] constexpr req<uint16_t> read_u16_be() {
    return read_u16().map(details::flip16);
  }
  [[nodiscard]] constexpr req<uint32_t> read_u32_be() {
    return read_u32().map(details::flip32);
  }
  [[nodiscard]] constexpr req<uint64_t> read_u64_be() {
    return read_u64().map(details::flip64);
  }

  [[nodiscard]] virtual constexpr req<uint64_t> size() {
    auto og = tellg();
    auto ng = seekg(0, seek_mode::end).fmap([this] { return tellg(); });
    return mno::combine(
               [this](auto og, auto ng) {
                 return seekg(og).map([ng] { return ng; });
               },
               og, ng)
        .fmap([](auto x) { return x; });
  }
};
} // namespace yoyo
