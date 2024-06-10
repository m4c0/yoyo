export module yoyo:subreader;
import :common;
import :reader;
import missingno;
import traits;

using namespace traits::ints;

namespace yoyo {
export class subreader : public reader {
  reader *m_o;
  uint64_t m_start;
  uint64_t m_len;

  constexpr subreader(reader *o, uint64_t start, uint64_t len)
      : m_o(o)
      , m_start(start)
      , m_len(len) {}

  [[nodiscard]] constexpr auto safe_read_up_to(unsigned d,
                                               auto fn) const noexcept {
    return tellg().fmap([this, fn, d](auto g) {
      auto len = static_cast<unsigned>(g + d < m_len ? d : m_len - g);
      return fn(len).map([len] { return len; });
    });
  }
  [[nodiscard]] constexpr auto safe_read(unsigned d, auto fn) const noexcept {
    return tellg()
        .assert([this, d](auto g) { return g + d <= m_len; }, "Buffer overflow")
        .fmap([fn](auto dg) { return fn(); });
  }

public:
  constexpr subreader() = default;

  [[nodiscard]] static constexpr req<subreader> create(subreader *o,
                                                       unsigned len) {
    return o->m_o->tellg().map(
        [=](auto start) { return subreader{o->m_o, start, len}; });
  }
  [[nodiscard]] static constexpr req<subreader>
  seek_and_create(subreader *o, unsigned start, unsigned len) {
    return o->m_o->seekg(start).map(
        [=] { return subreader{o->m_o, start, len}; });
  }

  [[nodiscard]] static constexpr req<subreader> create(reader *o,
                                                       unsigned len) {
    return o->tellg().map([=](auto start) { return subreader{o, start, len}; });
  }
  [[nodiscard]] static constexpr req<subreader>
  seek_and_create(reader *o, unsigned start, unsigned len) {
    return o->seekg(start).map([=] { return subreader{o, start, len}; });
  }

  [[nodiscard]] constexpr req<uint64_t> size() noexcept override {
    return req<uint64_t>{m_len};
  }
  [[nodiscard]] constexpr uint64_t raw_size() const noexcept { return m_len; }

  [[nodiscard]] constexpr req<bool> eof() const noexcept override {
    return tellg().map([this](auto g) { return g >= m_len; });
  }
  [[nodiscard]] constexpr req<unsigned>
  read_up_to(void *buffer, unsigned len) noexcept override {
    return safe_read_up_to(
        len, [this, buffer](auto l) { return m_o->read(buffer, l); });
  }
  [[nodiscard]] constexpr req<unsigned>
  read_up_to(uint8_t *buffer, unsigned len) noexcept override {
    return safe_read_up_to(
        len, [this, buffer](auto l) { return m_o->read(buffer, l); });
  }
  [[nodiscard]] constexpr req<void> read(void *buffer,
                                         unsigned len) noexcept override {
    return safe_read(len, [&] { return m_o->read(buffer, len); });
  }
  [[nodiscard]] constexpr req<void> read(uint8_t *buffer,
                                         unsigned len) noexcept override {
    return safe_read(len, [&] { return m_o->read(buffer, len); });
  }
  [[nodiscard]] constexpr req<uint8_t> read_u8() noexcept override {
    return safe_read(sizeof(uint8_t), [this] { return m_o->read_u8(); });
  }
  [[nodiscard]] constexpr req<uint16_t> read_u16() noexcept override {
    return safe_read(sizeof(uint16_t), [this] { return m_o->read_u16(); });
  }
  [[nodiscard]] constexpr req<uint32_t> read_u32() noexcept override {
    return safe_read(sizeof(uint32_t), [this] { return m_o->read_u32(); });
  }
  [[nodiscard]] constexpr req<uint64_t> read_u64() noexcept override {
    return safe_read(sizeof(uint64_t), [this] { return m_o->read_u64(); });
  }
  [[nodiscard]] constexpr req<void> seekg(int64_t pos) {
    if (pos < 0)
      return req<void>::failed("Attempt to seek before start of range");
    if (pos > m_len)
      return req<void>::failed("Attempt to seek past end of range");
    return m_o->seekg(m_start + pos);
  }
  [[nodiscard]] constexpr req<void> seekg(int64_t pos,
                                          seek_mode mode) noexcept override {
    switch (mode) {
    case seek_mode::set:
      return seekg(pos);
    case seek_mode::current:
      return m_o->tellg().fmap([&](auto g) {
        return seekg(g - static_cast<int64_t>(m_start) + pos);
      });
    case seek_mode::end:
      return seekg(m_len + pos);
    }
  }
  [[nodiscard]] constexpr req<uint64_t> tellg() const noexcept override {
    return req<uint64_t>{m_o->tellg() - m_start};
  }
};
} // namespace yoyo
