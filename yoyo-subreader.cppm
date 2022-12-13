module;
#include <optional>

export module yoyo:subreader;
import :reader;

namespace yoyo {
export class subreader : public reader {
  reader *m_o;
  unsigned m_start;
  unsigned m_len;

  constexpr subreader(reader *o, unsigned start, unsigned len)
      : m_o(o), m_start(start), m_len(len) {}

public:
  static constexpr std::optional<subreader>
  seek_and_create(reader *o, unsigned start, unsigned len) {
    if (!o->seekg(start))
      return {};
    return {subreader{o, start, len}};
  }

  [[nodiscard]] constexpr bool eof() const override { return tellg() >= m_len; }
  [[nodiscard]] constexpr bool read(void *buffer, unsigned len) override {
    if (tellg() + len > m_len)
      return false;
    return m_o->read(buffer, len);
  }
  [[nodiscard]] constexpr bool read(uint8_t *buffer, unsigned len) override {
    if (tellg() + len > m_len)
      return false;
    return m_o->read(buffer, len);
  }
  [[nodiscard]] constexpr std::optional<uint8_t> read_u8() override {
    if (tellg() + sizeof(uint8_t) > m_len)
      return {};
    return m_o->read_u8();
  }
  [[nodiscard]] constexpr std::optional<uint16_t> read_u16() override {
    if (tellg() + sizeof(uint16_t) > m_len)
      return {};
    return m_o->read_u16();
  }
  [[nodiscard]] constexpr std::optional<uint32_t> read_u32() override {
    if (tellg() + sizeof(uint32_t) > m_len)
      return {};
    return m_o->read_u32();
  }
  [[nodiscard]] constexpr bool seekg(unsigned pos) {
    if (pos < 0)
      return false;
    if (pos > m_len)
      return false;
    return m_o->seekg(m_start + pos);
  }
  [[nodiscard]] constexpr bool seekg(int pos, seek_mode mode) override {
    switch (mode) {
    case seek_mode::set:
      return seekg(pos);
    case seek_mode::current:
      return seekg(m_o->tellg() - m_start + pos);
    case seek_mode::end:
      return seekg(m_len + pos);
    }
  }
  [[nodiscard]] constexpr unsigned tellg() const override {
    return m_o->tellg() - m_start;
  }
};
} // namespace yoyo
