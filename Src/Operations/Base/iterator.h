#pragma once

#include "../../Operations/Base/sector.h"

static inline uintptr_t GetIteratorStartAddress(Iterator *iterator)
{
   return GetSectorStartAddress(iterator->Sector);
}

static inline uintptr_t GetIteratorEndAddress(Iterator *iterator)
{
   return GetSectorEndAddress(iterator->Sector);
}

static inline bool CanIteratorRead(Iterator *iterator)
{
   return iterator->ReadCursor >= GetIteratorStartAddress(iterator) &&
          iterator->ReadCursor <= GetIteratorEndAddress(iterator);
}

static inline bool CanIteratorWrite(Iterator *iterator)
{
   return iterator->WriteCursor >= GetIteratorStartAddress(iterator) &&
          iterator->WriteCursor <= GetIteratorEndAddress(iterator);
}

static inline void SeekIteratorReadCursor(Iterator *iterator, uintptr_t offset)
{
   iterator->ReadCursor = offset;
}

static inline void SeekIteratorWriteCursor(Iterator *iterator, uintptr_t offset)
{
   iterator->WriteCursor = offset;
}

static inline void IncrementIteratorReadCursor(Iterator *iterator)
{
   SeekIteratorReadCursor(iterator, GetNextBlockAddress(iterator->ReadCursor));
}

static inline void DecrementIteratorReadCursor(Iterator *iterator)
{
   SeekIteratorReadCursor(iterator, GetPreviousBlockAddress(iterator->ReadCursor));
}

static inline void IncrementIteratorWriteCursor(Iterator *iterator)
{
   SeekIteratorWriteCursor(iterator, GetNextBlockAddress(iterator->WriteCursor));
}

static inline void DecrementIteratorWriteCursor(Iterator *iterator)
{
   SeekIteratorWriteCursor(iterator, GetPreviousBlockAddress(iterator->WriteCursor));
}

static inline size_t GetIteratorEmptyBlocksCount(Iterator *iterator)
{
   if(CanIteratorWrite(iterator) == false)
      return 0;

   return 1 + (GetIteratorEndAddress(iterator) - iterator->WriteCursor) / GetBlocksSize(1);
}

static inline Iterator * GetNextSectorIterator(__SDEVICE_HANDLE(SimpleFs) *handle, Iterator *iterator)
{
   size_t nextIteratorIndex = (iterator - &handle->Runtime.Iterators[0] + 1) % __SIMPLE_FS_SDEVICE_SECTORS_COUNT;
   return &handle->Runtime.Iterators[nextIteratorIndex];
}

static inline bool IteratorTryPeek(__SDEVICE_HANDLE(SimpleFs) *handle, Iterator *iterator, Block *block)
{
   if(CanIteratorRead(iterator) != true)
      return false;

   ReadBlock(handle, iterator->ReadCursor, block);

   return true;
}

static inline bool IteratorTryReadForward(__SDEVICE_HANDLE(SimpleFs) *handle, Iterator *iterator, Block *block)
{
   if(CanIteratorRead(iterator) != true)
      return false;

   uintptr_t address = iterator->ReadCursor;
   IncrementIteratorReadCursor(iterator);

   ReadBlock(handle, address, block);

   return true;
}

static inline bool IteratorTryReadBackward(__SDEVICE_HANDLE(SimpleFs) *handle, Iterator *iterator, Block *block)
{
   if(CanIteratorRead(iterator) != true)
      return false;

   uintptr_t address = iterator->ReadCursor;
   DecrementIteratorReadCursor(iterator);

   ReadBlock(handle, address, block);

   return true;
}

static inline bool IteratorTryWriteForward(__SDEVICE_HANDLE(SimpleFs) *handle, Iterator *iterator, const Block *block)
{
   if(CanIteratorWrite(iterator) != true)
      return false;

   uintptr_t address = iterator->WriteCursor;
   IncrementIteratorWriteCursor(iterator);

   WriteBlock(handle, address, block);

   return true;
}

static inline bool IteratorTryReadBackToNotEmptyBlock(__SDEVICE_HANDLE(SimpleFs) *handle, Iterator *iterator)
{
   for(Block block; IteratorTryPeek(handle, iterator, &block) == true; DecrementIteratorReadCursor(iterator))
   {
      if(IsBlockEmpty(handle, &block) != true)
         return true;
   }

   return false;
}

static inline bool IteratorTryReadBackToBlockOfType(__SDEVICE_HANDLE(SimpleFs) *handle,
                                                    Iterator *iterator,
                                                    BlockDescriptor blockType)
{
   for(Block block; IteratorTryPeek(handle, iterator, &block) == true; DecrementIteratorReadCursor(iterator))
   {
      if(IsBlockOfType(&block, blockType) == true)
         return true;
   }

   return false;
}
