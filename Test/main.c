#include "Tests/Initialization/test_initialization.h"
#include "Tests/Operations/test_operations.h"

#include <stdio.h>
#include <string.h>

struct
{
   bool (*Function)(void);
   char *Description;
} TestFunctions[] =
{
   { TestHandleInitialization, "handle initialization" },

   { TestSmallWriteAndReadWithReinitialization, "small read and write" },
   { TestLargeWriteAndReadWithReinitialization, "large read and write" },
   { TestMultipleWriteAndReadWithReinitialization, "variable multiple read and write" },
   { TestMultipleVariablesWriteAndReadWithReinitialization, "multiple variables read and write" },
   { TestTransferWithReinitialization, "sectors transfer" }
};

const size_t TestFunctionsCount = sizeof(TestFunctions) / sizeof(*TestFunctions);

int main(void)
{
   int maxMessageLength = 0;
   for(size_t i = 0; i < TestFunctionsCount; i++)
   {
      int length = strlen(TestFunctions[i].Description);
      if(length > maxMessageLength)
         maxMessageLength = length;
   }

   printf("start tests...\n\n");

   for(size_t i = 0; i < TestFunctionsCount; i++)
      printf("%*s - %s\n",
             -maxMessageLength, TestFunctions[i].Description, TestFunctions[i].Function() == true ? "pass" : "fail");

   printf("\nstop tests...\n");
}
