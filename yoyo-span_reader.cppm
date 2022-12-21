module;
#include <optional>
#include <span>

export module yoyo:span_reader;
import :reader;

namespace yoyo {
export class span_reader : public reader {
  std::span<const uint8_t> m_data;
  std::span<const uint8_t> m_view;

  template <typename T> [[nodiscard]] std::optional<T> read_int() {
    T res{};
    return read(&res, sizeof(T)) ? std::optional<T>{res} : std::nullopt;
  }

public:
  constexpr explicit span_reader(std::span<const uint8_t> data)
      : m_data{data}, m_view{m_data} {}

  [[nodiscard]] bool read(void *buffer, unsigned len) override {
    return read((uint8_t *)buffer, len);
  }
  [[nodiscard]] constexpr bool read(uint8_t *buffer, unsigned len) override {
    if (len > m_view.size())
      return false;

    std::copy(m_view.begin(), m_view.begin() + len, buffer);
    m_view = m_view.subspan(len);
    return true;
  }
  [[nodiscard]] std::optional<uint8_t> read_u8() override {
    return read_int<uint8_t>();
  }
  [[nodiscard]] std::optional<uint16_t> read_u16() override {
    return read_int<uint16_t>();
  }
  [[nodiscard]] std::optional<uint32_t> read_u32() override {
    return read_int<uint32_t>();
  }
  [[nodiscard]] constexpr bool eof() const override { return m_view.empty(); }
  [[nodiscard]] constexpr bool seekg(unsigned pos) {
    if (pos < 0)
      return false;
    if (pos > m_data.size())
      return false;
    m_view = m_data.subspan(pos);
    return true;
  }
  [[nodiscard]] constexpr bool seekg(int pos, seek_mode mode) override {
    switch (mode) {
    case seek_mode::set:
      return seekg(pos);
    case seek_mode::current:
      return seekg(tellg() + pos);
    case seek_mode::end:
      return seekg(m_data.size() + pos);
    }
  }
  [[nodiscard]] constexpr unsigned tellg() const override {
    return m_data.size() - m_view.size();
  }
};
} // namespace yoyo
