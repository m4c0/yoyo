module;
#include <stdint.h>

export module yoyo:ce_reader;
import missingno;
import :reader;

namespace yoyo {
export template <auto N> class ce_reader : public reader {
  uint8_t m_data[N];
  unsigned m_pos{};

public:
  template <typename... Tp>
  constexpr explicit ce_reader(Tp... data)
      : m_data{static_cast<uint8_t>(data)...} {}
  constexpr explicit ce_reader(const char (&data)[N + 1]) {
    for (auto i = 0U; i < N; i++) {
      m_data[i] = data[i];
    }
  }

  [[nodiscard]] constexpr req<void> read(void * /*buffer*/,
                                         unsigned /*len*/) noexcept override {
    return req<void>::failed("CE doesn't support reading from generic pointer");
  }
  [[nodiscard]] constexpr req<void> read(uint8_t *buffer,
                                         unsigned len) noexcept override {
    req<void> res;
    for (auto i = 0U; i < len; i++) {
      res = res.fmap([this] { return read_u8(); }).map([&](auto b) {
        buffer[i] = b;
      });
    }
    return res;
  }
  [[nodiscard]] constexpr req<uint8_t> read_u8() noexcept override {
    if (eof())
      return req<uint8_t>::failed("Buffer underflow");
    return req<uint8_t>{m_data[m_pos++]};
  }
  [[nodiscard]] constexpr req<uint16_t> read_u16() noexcept override {
    constexpr const auto u8_bitsize = 8U;

    return read_u8().fmap([this](auto b) {
      return read_u8().map(
          [b](auto a) -> uint16_t { return (a << u8_bitsize) | b; });
    });
  }
  [[nodiscard]] constexpr req<uint32_t> read_u32() noexcept override {
    constexpr const auto u16_bitsize = 16U;

    return read_u16().fmap([this](auto b) {
      return read_u16().map(
          [b](auto a) -> uint32_t { return (a << u16_bitsize) | b; });
    });
  }
  [[nodiscard]] constexpr bool eof() const noexcept override {
    return m_pos >= N;
  }
  [[nodiscard]] constexpr req<void> seekg(unsigned pos) {
    if (pos < 0)
      return req<void>::failed("Attempt of seeking before start of buffer");
    if (pos > N)
      return req<void>::failed("Attempt of seeking past end of buffer");
    m_pos = pos;
    return {};
  }
  [[nodiscard]] constexpr req<void> seekg(int pos,
                                          seek_mode mode) noexcept override {
    switch (mode) {
    case seek_mode::set:
      return seekg(pos);
    case seek_mode::current:
      return seekg(m_pos + pos);
    case seek_mode::end:
      return seekg(N + pos);
    }
  }
  [[nodiscard]] constexpr unsigned tellg() const noexcept override {
    return m_pos;
  }
};
export template <typename... Tp> ce_reader(Tp...) -> ce_reader<sizeof...(Tp)>;
export template <auto N>
ce_reader(const char (&)[N]) -> ce_reader<N - 1>; // NOLINT
} // namespace yoyo
