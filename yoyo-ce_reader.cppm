export module yoyo:ce_reader;
import :common;
import :reader;
import traits;

using namespace traits::ints;

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

  [[nodiscard]] constexpr req<uint64_t> size() override {
    return req<uint64_t>{static_cast<uint64_t>(N)};
  }

  [[nodiscard]] constexpr req<unsigned> read_up_to(void * /*buffer*/,
                                                   unsigned /*len*/) override {
    return req<unsigned>::failed(
        "CE doesn't support reading from generic pointer");
  }
  [[nodiscard]] constexpr req<unsigned> read_up_to(uint8_t *buffer,
                                                   unsigned len) override {
    unsigned l = len + m_pos >= N ? N - m_pos : len;
    return read(buffer, l).map([l] { return l; });
  }

  [[nodiscard]] constexpr req<void> read(void * /*buffer*/,
                                         unsigned /*len*/) override {
    return req<void>::failed("CE doesn't support reading from generic pointer");
  }
  [[nodiscard]] constexpr req<void> read(uint8_t *buffer,
                                         unsigned len) override {
    req<void> res;
    for (auto i = 0U; i < len; i++) {
      res = res.fmap([this] { return read_u8(); }).map([&](auto b) {
        buffer[i] = b;
      });
    }
    return res;
  }
  [[nodiscard]] constexpr req<uint8_t> read_u8() override {
    return eof()
        .assert([](auto eof) { return !eof; }, "Buffer underflow")
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
    return req<bool>{m_pos >= N};
  }
  [[nodiscard]] constexpr req<void> seekg(unsigned pos) {
    if (pos < 0)
      return req<void>::failed("Attempt of seeking before start of buffer");
    if (pos > N)
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
      return seekg(N + pos);
    }
  }
  [[nodiscard]] constexpr req<uint64_t> tellg() const override {
    return req<uint64_t>{m_pos};
  }
};
export template <typename... Tp> ce_reader(Tp...) -> ce_reader<sizeof...(Tp)>;
export template <auto N>
ce_reader(const char (&)[N]) -> ce_reader<N - 1>; // NOLINT
} // namespace yoyo

using namespace yoyo;
static constexpr const ce_reader png{
    0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
    0x08, 0x02, 0x00, 0x00, 0x00, 0x90, 0x77, 0x53, 0xDE, 0x00, 0x00, 0x00,
    0x0C, 0x49, 0x44, 0x41, 0x54, 0x08, 0xD7, 0x63, 0xF8, 0xCF, 0xC0, 0x00,
    0x00, 0x03, 0x01, 0x01, 0x00, 0x18, 0xDD, 0x8D, 0xB0, 0x00, 0x00, 0x00,
    0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82};
// Test initial state
static_assert([] {
  ce_reader dat = png;
  return (dat.eof() == false) && (dat.tellg() == 0);
}());

// Test if we can seek
static_assert([] {
  constexpr const auto valid_pos = 10;
  ce_reader dat = png;
  return dat.seekg(valid_pos)
      .map([&] { return (dat.eof() == false) && (dat.tellg() == valid_pos); })
      .unwrap(false);
}());

// Test if we cannot seek past end
static_assert([] {
  constexpr const auto invalid_pos = 1000;
  ce_reader dat = png;
  return !dat.seekg(invalid_pos).map([] { return true; }).unwrap(false);
}());

// Test if we can seek backwards
static_assert([] {
  constexpr const auto valid_pos = 10;
  constexpr const auto valid_rewind = 3;
  ce_reader dat = png;
  return dat.seekg(valid_pos)
      .fmap([&] { return dat.seekg(valid_rewind); })
      .map(
          [&] { return (dat.eof() == false) && (dat.tellg() == valid_rewind); })
      .unwrap(false);
}());

// Test if we cannot seek before start
static_assert([] {
  constexpr const auto invalid_pos = -1;
  ce_reader dat = png;
  return dat.seekg(invalid_pos).map([] { return false; }).unwrap(true);
}());

// Test if we can seek using mode::cur
static_assert([] {
  constexpr const auto any_pos = 10;
  constexpr const auto delta_fwd = 5;
  constexpr const auto delta_rev = -2;
  constexpr const auto final_pos = any_pos + delta_fwd + delta_rev;

  ce_reader dat = png;
  return dat.seekg(any_pos)
      .fmap([&] { return dat.seekg(delta_fwd, seek_mode::current); })
      .fmap([&] { return dat.seekg(delta_rev, seek_mode::current); })
      .map([&] { return dat.tellg() == final_pos; })
      .unwrap(false);
}());

// Test if we can seek using mode::end
static_assert([] {
  ce_reader dat('R', 'I', 'F', 'F');
  return dat.seekg(-1, seek_mode::end)
      .map([&] { return dat.tellg() == 3; })
      .unwrap(false);
}());

// Test if it seeks when reading
static_assert([] {
  constexpr const auto final_pos = 7;

  ce_reader dat = png;
  return dat.seekg(3)
      .fmap([&] { return dat.read_u32(); })
      .map([&](auto) { return dat.tellg() == final_pos; })
      .unwrap(false);
}());

// Test if we can read in proper endianness
static_assert([] {
  constexpr const auto expected = 13U;
  constexpr const auto ihdr_pos = 8;

  ce_reader dat = png;
  return dat.seekg(ihdr_pos)
      .fmap([&] { return dat.read_u32_be(); })
      .map([](auto d) { return d == expected; })
      .unwrap(false);
}());
static_assert([] {
  return ce_reader('R', 'I', 'F', 'F')
      .read_u32()
      .map([](auto d) { return d == 'FFIR'; })
      .unwrap(false);
}());

// Test if we don't read past the end
static_assert([] {
  ce_reader dat('R', 'I', 'F', 'F');
  return dat.seekg(2)
      .fmap([&] { return dat.read_u32(); })
      .map([](auto) { return false; })
      .unwrap(true);
}());

// Test if we can build from a string literal
static_assert([] {
  return ce_reader("RIFF")
      .read_u32()
      .map([](auto d) { return d == 'FFIR'; })
      .unwrap(false);
}());

// Test if we can seek to EOF
static_assert([] {
  ce_reader dat("RIFF");
  return dat.seekg(4).fmap([&] { return dat.eof(); }).unwrap(false);
}());

// Test if we can find the size of a stream
static_assert([] {
  ce_reader dat("I'm fine");
  return dat.seekg(3).fmap([&] { return dat.size(); }).unwrap(0) == 8;
}());

// Test if "size" doesn't change the head position
static_assert([] {
  ce_reader dat("I'm fine");
  return dat.seekg(3)
             .fmap([&] { return dat.size(); })
             .fmap([&](auto) { return dat.tellg(); })
             .unwrap(0) == 3;
}());

// Test if can "read"
static_assert([] {
  uint8_t buf[2]{};
  ce_reader dat("OK");
  return dat.read(buf, 2)
      .map([&] { return buf[0] == 'O' && buf[1] == 'K'; })
      .unwrap(false);
}());

// Test if can "read up to" a size
static_assert([] {
  uint8_t buf[2]{};
  ce_reader dat("OK");
  return dat.read_up_to(buf, 5)
      .map([&](auto len) { return len == 2 && buf[0] == 'O' && buf[1] == 'K'; })
      .unwrap(false);
}());
