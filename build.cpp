#include "build.hpp"

using namespace ecow;

int main(int argc, char **argv) {
  seq all{"all"};

  auto m = yoyo();
  all.add_ref(m);

  auto tst = all.add_unit<exe>("tests");
  tst->add_unit<>("subreader-test");
  tst->add_ref(m);

  return run_main(all, argc, argv);
}
