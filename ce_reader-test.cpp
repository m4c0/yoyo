module;
#include <array>
#include <optional>

module yoyo;
import :ce_reader;

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
  return !dat.eof() && (dat.tellg() == 0);
}());

// Test if we can seek
static_assert([] {
  constexpr const auto valid_pos = 10;
  ce_reader dat = png;
  return dat.seekg(valid_pos) && !dat.eof() && (dat.tellg() == valid_pos);
}());

// Test if we cannot seek past end
static_assert([] {
  constexpr const auto invalid_pos = 1000;
  ce_reader dat = png;
  return !dat.seekg(invalid_pos);
}());

// Test if we can seek backwards
static_assert([] {
  constexpr const auto valid_pos = 10;
  constexpr const auto valid_rewind = 3;
  ce_reader dat = png;
  return dat.seekg(valid_pos) && dat.seekg(valid_rewind) && !dat.eof() &&
         (dat.tellg() == valid_rewind);
}());

// Test if we cannot seek before start
static_assert([] {
  constexpr const auto invalid_pos = -1;
  ce_reader dat = png;
  return !dat.seekg(invalid_pos);
}());

// Test if we can seek using mode::cur
static_assert([] {
  constexpr const auto any_pos = 10;
  constexpr const auto delta_fwd = 5;
  constexpr const auto delta_rev = -2;
  constexpr const auto final_pos = any_pos + delta_fwd + delta_rev;

  ce_reader dat = png;
  return dat.seekg(any_pos) && dat.seekg(delta_fwd, seek_mode::current) &&
         dat.seekg(delta_rev, seek_mode::current) && (dat.tellg() == final_pos);
}());

// Test if we can seek using mode::end
static_assert([] {
  ce_reader dat('R', 'I', 'F', 'F');
  return dat.seekg(-1, seek_mode::end) && dat.tellg() == 3;
}());

// Test if it seeks when reading
static_assert([] {
  constexpr const auto final_pos = 7;

  ce_reader dat = png;
  return dat.seekg(3) && dat.read_u32() && dat.tellg() == final_pos;
}());

// Test if we can read in proper endianness
static_assert([] {
  constexpr const auto expected = 13U;
  constexpr const auto ihdr_pos = 8;

  ce_reader dat = png;
  return dat.seekg(ihdr_pos) && (dat.read_u32_be() == expected);
}());
static_assert([] {
  return ce_reader('R', 'I', 'F', 'F').read_u32() == 'FFIR';
}());

// Test if we don't read past the end
static_assert([] {
  ce_reader dat('R', 'I', 'F', 'F');
  return dat.seekg(2) && !dat.read_u32();
}());

// Test if we can build from a string literal
static_assert([] { return ce_reader("RIFF").read_u32() == 'FFIR'; }());

// Test if we can seek to EOF
static_assert([] {
  ce_reader dat("RIFF");
  return dat.seekg(4) && dat.eof();
}());

int main() {}
