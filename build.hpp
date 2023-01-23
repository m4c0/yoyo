#pragma once

#include "../hai/build.hpp"
#include "../missingno/build.hpp"
#include "ecow.hpp"

auto yoyo() {
  using namespace ecow;
  auto m = unit::create<mod>("yoyo");
  m->add_wsdep("hai", hai());
  m->add_wsdep("missingno", missingno());
  m->add_part("reader");
  m->add_part("writer");
  m->add_part("ce_reader");
  m->add_part("subreader");
  m->add_impl("ce_reader-test");
  return m;
}

auto yoyo_stl() {
  using namespace ecow;
  auto m = unit::create<mod>("yoyo_stl");
  m->add_ref(yoyo());
  m->add_part("istr_reader");
  m->add_part("ostr_writer");
  m->add_part("span_reader");
  return m;
}

auto yoyo_libc() {
  using namespace ecow;
  auto m = unit::create<mod>("yoyo_libc");
  m->add_wsdep("hai", hai());
  m->add_wsdep("missingno", missingno());
  m->add_ref(yoyo());
  m->add_part("file_reader");
  return m;
}
