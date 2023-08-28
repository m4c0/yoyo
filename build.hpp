#pragma once

#include "ecow.hpp"

auto yoyo() {
  using namespace ecow;
  return unit::create<box>("yoyo");
}

auto yoyo_libc() {
  using namespace ecow;
  return unit::create<box>("yoyo_libc");
}
