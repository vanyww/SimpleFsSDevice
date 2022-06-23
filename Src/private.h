#include "../Inc/FlashFileSystemSDevice/core.h"

typedef struct
{
   size_t SectorIndex;
   uintptr_t WriteCursor;
   uintptr_t ReadCursor;
} FlashFileSystemIterator;

typedef struct
{
   FlashFileSystemAddress Address;
   uintptr_t MemoryAddress;
   uint8_t Size;
   bool IsDeleted;
} FlashFileSystemFileDataCache;

struct __SDEVICE_RUNTIME_DATA(FlashFileSystem)
{
   size_t ActiveIteratorIndex;
   FlashFileSystemIterator Iterators[__FLASH_FILE_SYSTEM_SECTORS_COUNT];
   FlashFileSystemFileDataCache FileDataCache;
};
