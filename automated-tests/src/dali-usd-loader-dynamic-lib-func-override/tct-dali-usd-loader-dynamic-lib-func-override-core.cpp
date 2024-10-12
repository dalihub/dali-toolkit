#include <test-harness.h>

// Must come second
#include "tct-dali-usd-loader-dynamic-lib-func-override-core.h"

int main(int argc, char* const argv[])
{
  return TestHarness::RunTests(argc, argv, tc_array);
}
