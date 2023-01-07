module;
#include <optional>

export module yoyo:subreader;
import missingno;
import :reader;

namespace yoyo {
export class subreader : public reader {
  reader *m_o;
  unsigned m_start;
  unsigned m_len;

  constexpr subreader(reader *o, unsigned start, unsigned len)
      : m_o(o), m_start(start), m_len(len) {}

public:
  constexpr subreader() = default;

  static constexpr req<subreader> seek_and_create(reader *o, unsigned start,
                                                  unsigned len) {
    return o->seekg(start).map([=] { return subreader{o, start, len}; });
  }

  [[nodiscard]] constexpr bool eof() const noexcept override {
    return tellg() >= m_len;
  }
  [[nodiscard]] constexpr req<void> read(void *buffer,
                                         unsigned len) noexcept override {
    if (tellg() + len > m_len)
      return req<void>::failed("Buffer overflow");
    return m_o->read(buffer, len);
  }
  [[nodiscard]] constexpr req<void> read(uint8_t *buffer,
                                         unsigned len) noexcept override {
    if (tellg() + len > m_len)
      return req<void>::failed("Buffer overflow");
    return m_o->read(buffer, len);
  }
  [[nodiscard]] constexpr req<uint8_t> read_u8() noexcept override {
    if (tellg() + sizeof(uint8_t) > m_len)
      return req<uint8_t>::failed("Buffer overflow");
    return m_o->read_u8();
  }
  [[nodiscard]] constexpr req<uint16_t> read_u16() noexcept override {
    if (tellg() + sizeof(uint16_t) > m_len)
      return req<uint16_t>::failed("Buffer overflow");
    return m_o->read_u16();
  }
  [[nodiscard]] constexpr req<uint32_t> read_u32() noexcept override {
    if (tellg() + sizeof(uint32_t) > m_len)
      return req<uint32_t>::failed("Buffer overflow");
    return m_o->read_u32();
  }
  [[nodiscard]] constexpr req<void> seekg(unsigned pos) {
    if (pos < 0)
      return req<void>::failed("Buffer overflow");
    if (pos > m_len)
      return req<void>::failed("Buffer underflow");
    return m_o->seekg(m_start + pos);
  }
  [[nodiscard]] constexpr req<void> seekg(int pos,
                                          seek_mode mode) noexcept override {
    switch (mode) {
    case seek_mode::set:
      return seekg(pos);
    case seek_mode::current:
      return seekg(m_o->tellg() - m_start + pos);
    case seek_mode::end:
      return seekg(m_len + pos);
    }
  }
  [[nodiscard]] constexpr unsigned tellg() const noexcept override {
    return m_o->tellg() - m_start;
  }
};
} // namespace yoyo
