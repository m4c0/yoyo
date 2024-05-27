#pragma leco tool

import yoyo;

using namespace yoyo;

static auto fail() { return false; }

class holder {
  ce_reader<14> m_reader{"this is a test"};
  mno::req<subreader> m_sub;

public:
  static constexpr auto whole_len = 14;
  static constexpr auto start_pos = 5; // "is a"
  static constexpr auto len = 4;

  constexpr holder() : holder(start_pos, len) {}
  constexpr holder(unsigned len)
      : m_sub{m_reader.seekg(start_pos).fmap(
            [&] { return subreader::create(&m_reader, len); })} {}
  constexpr holder(unsigned start, unsigned len)
      : m_sub(subreader::seek_and_create(&m_reader, start, len)) {}

  constexpr auto &sub() {
    return m_sub; }
  constexpr const auto &underlying() const {
    return m_reader; }

  constexpr auto assert_position(bool eof, unsigned pos) {
    return m_sub.map(
        [=](auto sub) { return (sub.tellg() == pos) && (sub.eof() == eof); });
  }

  template <auto N> constexpr auto read(char (&buf)[N]) {
    return m_sub.fmap([&](auto sub) { return sub.read(buf, N); });
  }

  constexpr auto seek(auto offs) {
    return m_sub.fmap([offs](auto sub) { return sub.seekg(offs); });
  }
  constexpr auto seek(auto offs, auto mode) {
    return m_sub.fmap([offs, mode](auto sub) { return sub.seekg(offs, mode); });
  }
};

static constexpr auto assert_position(holder &h, bool eof, unsigned pos) {
  return [&h, eof, pos] { return h.assert_position(eof, pos); };
}

// seeks stream in ctor
static_assert([] {
  holder h{2, 1};
  return h.sub()
      .map([&](auto) { return h.underlying().tellg() == 2; })
      .unwrap(false);
}());
static_assert([] {
  holder h{2, 1};
  unsigned char buf[4];
  return h.sub()
      .fmap([&](auto s) { return s.read(buf, 1); })
      .map([&] { return buf[0] == 'i'; })
      .unwrap(false);
}());

// creates an invalid instance if ctor can't seek
static_assert(holder{holder::whole_len + 1, 1}
                  .sub()
                  .map([](auto) { return fail(); })
                  .unwrap(true));

// can tell current position from start
static_assert(holder{}.assert_position(false, 0).unwrap(false));
static_assert(holder{2}.assert_position(false, 0).unwrap(false));

// doesn't seek before limit
static_assert([] {
  holder h{};
  auto s = h.seek(-1).map(fail).otherwise(true);
  auto p = h.assert_position(false, 0);
  return s.unwrap(false) && p.unwrap(false);
}());

// doesn't seek past limit
static_assert([] {
  holder h{};
  auto s = h.seek(holder::len + 1).map(fail).otherwise(true);
  auto p = h.assert_position(false, 0);
  return s.unwrap(false) && p.unwrap(false);
}());

// seeks properly
static_assert([] {
  holder h;
  return h.seek(2).fmap(assert_position(h, false, 2)).unwrap(false);
}());

// seeks to current pos
static_assert([] {
  holder h;
  return h.seek(0).fmap(assert_position(h, false, 0)).unwrap(false);
}());

// seeks from current position
static_assert([] {
  holder h;
  return h.seek(2).fmap(assert_position(h, false, 2)).unwrap(false) &&
         h.seek(1, seek_mode::current)
             .fmap(assert_position(h, false, 3))
             .unwrap(false) &&
         h.seek(-2, seek_mode::current)
             .fmap(assert_position(h, false, 1))
             .unwrap(false);
}());

// seeks from end
static_assert([] {
  holder h;
  return h.seek(-1, seek_mode::end)
      .fmap(assert_position(h, false, holder::len - 1))
      .unwrap(false);
}());

// subreader of a dead subreader
static_assert([] {
  auto r = ce_reader{"testing"};
  return yoyo::subreader::create(&r, 4)
      .fmap([](auto sr1) { return yoyo::subreader::create(&sr1, 2); })
      .fmap([](auto sr2) { return sr2.read_u32(); })
      .unwrap(0);
}() > 0);

// reads
static_assert([] {
  char buf[] = "12";
  return holder{}
      .read(buf)
      .map([&] { return buf[0] == 'i' && buf[1] == 's'; })
      .unwrap(false);
});

// reads all
static_assert([] {
  char buf[] = "1234";
  return holder{}
      .read(buf)
      .map([&] {
        return buf[0] == 'i' && buf[1] == 's' && buf[2] == ' ' && buf[3] == 'a';
      })
      .unwrap(false);
});

// doesnt read past limit
static_assert([] {
  char buf[] = "12345";
  holder h;
  return h.read(buf).map(fail).unwrap(true) && buf[0] == '1' && buf[1] == '2' &&
         buf[2] == '3' && buf[3] == '4' && buf[4] == '5';
});

int main() {}
