#include "Mock/SimpleFs/Data/TestDataBuilder/data_builder.h"

#include "unity_fixture.h"

static void RunAllTests(void)
{
   RUN_TEST_GROUP(InitializationTests);
   RUN_TEST_GROUP(ReadWrite);
   RUN_TEST_GROUP(CommonSimpleFs);
   RUN_TEST_GROUP(Transfer);
   RUN_TEST_GROUP(BadArea);
}

int main (int argc, char const *argv[])
{
   CreateTestData();
   UnityMain (argc, argv, RunAllTests);
   return 0;
}
