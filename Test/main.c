#include "unity_fixture.h"

static void RunAllTests(void) {}

int main (int argc, char const *argv[])
{
   UnityMain (argc, argv, RunAllTests);
   return 0;
}
