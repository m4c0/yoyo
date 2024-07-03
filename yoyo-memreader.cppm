export module yoyo:memreader;
import :common;
import :reader;
import traits;

using namespace traits::ints;

namespace yoyo {
export class memreader : public reader {
  const uint8_t *m_data{};
  unsigned m_size{};
  unsigned m_pos{};

public:
  constexpr explicit memreader(const uint8_t *d, unsigned s)
      : m_data{d}
      , m_size{s} {}

  [[nodiscard]] constexpr req<uint64_t> size() override {
    return req<uint64_t>{static_cast<uint64_t>(m_size)};
  }

  [[nodiscard]] constexpr req<unsigned> read_up_to(uint8_t *buffer,
                                                   unsigned len) override {
    unsigned l = len + m_pos >= m_size ? m_size - m_pos : len;
    return read(buffer, l).map([l] { return l; });
  }
  [[nodiscard]] constexpr req<unsigned> read_up_to(void *buffer,
                                                   unsigned len) override {
    return read_up_to(static_cast<uint8_t *>(buffer), len);
  }

  [[nodiscard]] constexpr req<void> read(uint8_t *buffer,
                                         unsigned len) override {
    if (len + m_pos >= m_size)
      return req<void>::failed("buffer underflow");

    for (auto i = 0U; i < len; i++) {
      buffer[i] = m_data[m_pos++];
    }
    return {};
  }
  [[nodiscard]] constexpr req<void> read(void *buffer, unsigned len) override {
    return read(static_cast<uint8_t *>(buffer), len);
  }
  [[nodiscard]] constexpr req<uint8_t> read_u8() override {
    return eof()
        .assert([](auto eof) { return !eof; }, "buffer underflow")
        .map([&](auto) { return m_data[m_pos++]; });
  }
  [[nodiscard]] constexpr req<uint16_t> read_u16() override {
    constexpr const auto u8_bitsize = 8U;

    return read_u8().fmap([this](auto b) {
      return read_u8().map(
          [b](auto a) -> uint16_t { return (a << u8_bitsize) | b; });
    });
  }
  [[nodiscard]] constexpr req<uint32_t> read_u32() override {
    constexpr const auto u16_bitsize = 16U;

    return read_u16().fmap([this](auto b) {
      return read_u16().map(
          [b](auto a) -> uint32_t { return (a << u16_bitsize) | b; });
    });
  }
  [[nodiscard]] constexpr req<uint64_t> read_u64() override {
    constexpr const auto u32_bitsize = 32U;

    return read_u32().fmap([this](auto b) {
      return read_u32().map([b](auto a) -> uint64_t {
        return (static_cast<uint64_t>(a) << u32_bitsize) | b;
      });
    });
  }
  [[nodiscard]] constexpr req<bool> eof() const override {
    return req<bool>{m_pos >= m_size};
  }
  [[nodiscard]] constexpr req<void> seekg(unsigned pos) {
    if (pos < 0)
      return req<void>::failed("Attempt of seeking before start of buffer");
    if (pos > m_size)
      return req<void>::failed("Attempt of seeking past end of buffer");
    m_pos = pos;
    return {};
  }
  [[nodiscard]] constexpr req<void> seekg(int64_t pos,
                                          seek_mode mode) override {
    switch (mode) {
    case seek_mode::set:
      return seekg(pos);
    case seek_mode::current:
      return seekg(m_pos + pos);
    case seek_mode::end:
      return seekg(m_size + pos);
    }
  }
  [[nodiscard]] constexpr req<uint64_t> tellg() const override {
    return req<uint64_t>{m_pos};
  }

  [[nodiscard]] constexpr auto raw_pos() const { return m_pos; }
};
} // namespace yoyo
