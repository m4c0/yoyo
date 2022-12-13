#include <array>
#include <cassert>
#include <optional>
#include <sstream>

import yoyo;

using namespace yoyo;

class holder {
  std::istringstream m_str;
  istr_reader m_reader;
  std::optional<subreader> m_sub;

public:
  static constexpr auto start_pos = 5; // "is a"
  static constexpr auto len = 4;

  holder() : holder("this is a test", start_pos, len) {}
  holder(const char *text, unsigned start, unsigned len)
      : m_str(text), m_reader(m_str),
        m_sub(subreader::seek_and_create(&m_reader, start, len)) {}

  [[nodiscard]] constexpr explicit operator bool() const noexcept {
    return m_sub.has_value();
  }
  [[nodiscard]] constexpr auto *operator->() noexcept { return &m_sub.value(); }

  [[nodiscard]] constexpr auto &strstream() noexcept { return m_str; }
};

static void assert_position(holder &h, bool eof, unsigned pos) {
  assert(static_cast<bool>(h));
  assert(h->tellg() == pos);
  assert(h->eof() == eof);
}
static void assert_position(bool result, holder &h, bool eof, unsigned pos) {
  assert(result);
  assert_position(h, eof, pos);
}

int main() {
  { // seeks stream in ctor
    holder h{"test", 2, 1};
    assert(static_cast<bool>(h));
    assert(static_cast<unsigned>(h.strstream().tellg()) == 2);
  }
  { // creates an invalid instance if ctor can't seek
    holder h{"test", holder::len + 1, 1};
    assert(!static_cast<bool>(h));
  }
  { // can tell current position from start
    holder h;
    assert_position(h, false, 0);
  }
  { // doesn't seek before limit
    holder h;
    assert_position(!h->seekg(-1), h, false, 0);
  }
  { // doesn't seek past limit
    holder h;
    assert_position(!h->seekg(holder::len + 1), h, false, 0);
  }
  { // seeks properly
    holder h;
    assert_position(h->seekg(2), h, false, 2);
  }
  { // seeks to current pos
    holder h;
    assert_position(h->seekg(0), h, false, 0);
  }
  { // seeks from current position
    holder h;
    assert_position(h->seekg(2), h, false, 2);
    assert_position(h->seekg(1, seek_mode::current), h, false, 3);
    assert_position(h->seekg(-2, seek_mode::current), h, false, 1);
  }
  { // seeks from end
    holder h;
    assert_position(h->seekg(-1, seek_mode::end), h, false, 3);
  }
  { // reads
    holder h;
    std::string buf{"12"};
    assert(h->read(buf.data(), buf.size()));
    assert(buf == "is");
  }
  { // reads all
    holder h;
    std::string buf{"1234"};
    assert(h->read(buf.data(), buf.size()));
    assert(buf == "is a");
  }
  { // doesnt read past limit
    holder h;
    std::string buf{"12345"};
    assert(!h->read(buf.data(), buf.size()));
    assert(buf == "12345");
  }
}
