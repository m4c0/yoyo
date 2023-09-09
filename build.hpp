#pragma once

#include "../ecow/ecow.hpp"

auto yoyo() {
  using namespace ecow;
  return unit::create<box>("yoyo");
}
