module;
#include <istream>
#include <optional>

export module yoyo:istr_reader;
import :reader;

namespace yoyo {
export class istr_reader : public reader {
  std::istream &m_ifs;

public:
  explicit constexpr istr_reader(std::istream &ifs) : m_ifs(ifs) {}

  [[nodiscard]] bool eof() const override { return m_ifs.eof(); }

  [[nodiscard]] bool read(void *buffer, unsigned len) override {
    return static_cast<bool>(m_ifs.read(static_cast<char *>(buffer), len));
  }
  [[nodiscard]] std::optional<uint32_t> read_u32() override {
    uint32_t res{};
    if (read(&res, sizeof(uint32_t)))
      return res;
    return {};
  }
  [[nodiscard]] std::optional<uint16_t> read_u16() override {
    uint16_t res{};
    if (read(&res, sizeof(uint16_t)))
      return res;
    return {};
  }
  [[nodiscard]] std::optional<uint8_t> read_u8() override {
    uint8_t res{};
    if (read(&res, sizeof(uint8_t)))
      return res;
    return {};
  }

  [[nodiscard]] bool seekg(int pos, seek_mode mode) override {
    auto dir = static_cast<std::ios::seekdir>(mode);
    return static_cast<bool>(m_ifs.seekg(pos, dir));
  }
  [[nodiscard]] unsigned tellg() const override { return m_ifs.tellg(); }
};
} // namespace yoyo
