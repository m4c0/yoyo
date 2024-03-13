export module yoyo:reader;
import :common;
import missingno;
import traits;

namespace yoyo {
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
  [[nodiscard]] virtual req<unsigned> read_up_to(uint8_t *buffer,
                                                 unsigned len) noexcept {
    return read_up_to(static_cast<void *>(buffer), len);
  }
  [[nodiscard]] virtual req<unsigned> read_up_to(void *buffer,
                                                 unsigned len) noexcept = 0;

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
  [[nodiscard]] virtual req<uint64_t> read_u64() noexcept {
    return read<uint64_t>();
  }

  [[nodiscard]] constexpr req<uint16_t> read_u16_be() noexcept {
    return read_u16().map(details::flip16);
  }
  [[nodiscard]] constexpr req<uint32_t> read_u32_be() noexcept {
    return read_u32().map(details::flip32);
  }
  [[nodiscard]] constexpr req<uint64_t> read_u64_be() noexcept {
    return read_u64().map(details::flip64);
  }

  [[nodiscard]] virtual constexpr req<unsigned> size() noexcept {
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
