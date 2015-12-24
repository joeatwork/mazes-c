#include <glib.h>
#include <stdio.h>
#include <string.h>

void test_runs() {
  g_assert(1 == 1);
}

int main(int argc, char** argv) {
  g_test_init(&argc, &argv, NULL);

  g_test_add_func("/maze/test_runs", test_runs);
  return g_test_run();
}
