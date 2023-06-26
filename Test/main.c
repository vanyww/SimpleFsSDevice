#include "unity_fixture.h"

#include "TestDataBuilder/test_data.h"

static void RunAllTests(void)
{
   RUN_TEST_GROUP(InitializationTests);
   RUN_TEST_GROUP(ReadWrite);
   RUN_TEST_GROUP(CommonSimpleFs);
}

int main (int argc, char const *argv[])
{
   CreateTestData();
   UnityMain (argc, argv, RunAllTests);
   return 0;
}
