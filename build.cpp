#include "ecow.hpp"

using namespace ecow;

int main(int argc, char **argv) {
  seq all{"all"};

  auto m = all.add_unit<mod>("yoyo");
  m->add_part("reader");
  m->add_part("writer");
  m->add_part("ce_reader");
  m->add_part("istr_reader");
  m->add_part("ostr_writer");
  m->add_part("subreader");
  m->add_impl("ce_reader-test");

  auto tst = all.add_unit<exe>("tests");
  tst->add_unit<>("subreader-test");
  tst->add_ref(m);

  return run_main(all, argc, argv);
}
