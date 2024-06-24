export module yoyo:memwriter;
import :common;
import :writer;
import hai;
import missingno;
import traits;

namespace yoyo {
export class memwriter : public writer {
  uint8_t *m_buffer{};
  unsigned m_pos{};
  unsigned m_size{};

public:
  constexpr memwriter() = default;
  constexpr memwriter(hai::array<uint8_t> &buf)
      : m_buffer{buf.begin()}
      , m_size{buf.size()} {}

  [[nodiscard]] virtual mno::req<void> seekp(int pos, seek_mode mode) {
    switch (mode) {
    case seek_mode::set:
      break;
    case seek_mode::current:
      pos += m_pos;
      break;
    case seek_mode::end:
      pos = m_size + pos;
      break;
    }
    if (pos < 0)
      return mno::req<void>::failed("attempt of skipping past start of string");
    if (pos >= m_size)
      return mno::req<void>::failed("attempt of skipping past end of string");

    m_pos = pos;
    return {};
  }
  [[nodiscard]] virtual mno::req<unsigned> tellp() const {
    return mno::req{m_pos};
  }

  [[nodiscard]] virtual mno::req<void> write(const void *buffer, unsigned len) {
    if (len + m_pos > m_size)
      return mno::req<void>::failed("attempt of writing past end of string");

    auto buf = static_cast<const uint8_t *>(buffer);
    for (auto i = 0; i < len; i++) {
      m_buffer[m_pos++] = *buf++;
    }
    return {};
  }
};
} // namespace yoyo
