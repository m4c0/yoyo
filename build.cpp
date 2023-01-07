#include "build.hpp"

using namespace ecow;

int main(int argc, char **argv) {
  auto tst = unit::create<exe>("tests");
  tst->add_ref(yoyo());
  tst->add_unit<>("subreader-test");
  return run_main(tst, argc, argv);
}
