#pragma once

#include "sector.h"

static inline intptr_t IteratorStart(FlashFileSystemIterator *iterator)
{
   return SectorFirstBlockAddress(iterator->Sector);
}

static inline intptr_t IteratorEnd(FlashFileSystemIterator *iterator)
{
   return SectorLastBlockAddress(iterator->Sector);
}

static inline bool CanRead(FlashFileSystemIterator *iterator)
{
   return iterator->ReadCursor >= IteratorStart(iterator) && iterator->ReadCursor <= IteratorEnd(iterator);
}

static inline bool CanWrite(FlashFileSystemIterator *iterator)
{
   return iterator->WriteCursor >= IteratorStart(iterator) && iterator->WriteCursor <= IteratorEnd(iterator);
}

static inline void SeekReadCursor(FlashFileSystemIterator *iterator, intptr_t offset)
{
   iterator->ReadCursor = offset;
}

static inline void SeekWriteCursor(FlashFileSystemIterator *iterator, intptr_t offset)
{
   iterator->WriteCursor = offset;
}

static inline void IncrementReadCursor(FlashFileSystemIterator *iterator)
{
   SeekReadCursor(iterator, NextBlockAddress(iterator->ReadCursor));
}

static inline void DecrementReadCursor(FlashFileSystemIterator *iterator)
{
   SeekReadCursor(iterator, PreviousBlockAddress(iterator->ReadCursor));
}

static inline void IncrementWriteCursor(FlashFileSystemIterator *iterator)
{
   SeekWriteCursor(iterator, NextBlockAddress(iterator->WriteCursor));
}

static inline void DecrementWriteCursor(FlashFileSystemIterator *iterator)
{
   SeekWriteCursor(iterator, PreviousBlockAddress(iterator->WriteCursor));
}

static inline size_t EmptyBlocksCount(FlashFileSystemIterator *iterator)
{
   if(CanWrite(iterator) == false)
      return 0;

   return 1 + (SectorLastBlockAddress(iterator->Sector) - iterator->WriteCursor) / BlocksSize(1);
}

static inline FlashFileSystemStatus PeekFromCurrentBlock(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                        FlashFileSystemIterator *iterator,
                                                        FileSystemBlock *block)
{
   if(CanRead(iterator) != true)
      return FLASH_FILE_SYSTEM_STATUS_VALUE_NOT_FOUND_ERROR;

   return ReadBlock(handle, iterator->ReadCursor, block);
}

static inline FlashFileSystemStatus ReadForwardFromCurrentBlock(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                               FlashFileSystemIterator *iterator,
                                                               FileSystemBlock *block)
{
   if(CanRead(iterator) != true)
      return FLASH_FILE_SYSTEM_STATUS_VALUE_NOT_FOUND_ERROR;

   intptr_t address = iterator->ReadCursor;
   IncrementReadCursor(iterator);

   return ReadBlock(handle, address, block);
}

static inline FlashFileSystemStatus ReadBackwardFromCurrentBlock(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                                FlashFileSystemIterator *iterator,
                                                                FileSystemBlock *block)
{
   if(CanRead(iterator) != true)
      return FLASH_FILE_SYSTEM_STATUS_VALUE_NOT_FOUND_ERROR;

   intptr_t address = iterator->ReadCursor;
   DecrementReadCursor(iterator);

   return ReadBlock(handle, address, block);
}

static inline FlashFileSystemStatus WriteForwardToCurrentBlock(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                              FlashFileSystemIterator *iterator,
                                                              const FileSystemBlock *block)
{
   if(CanWrite(iterator) != true)
   {
      SDeviceRuntimeErrorRaised(handle, FLASH_FILE_SYSTEM_RUNTIME_ERROR_OUT_OF_MEMORY);
      return FLASH_FILE_SYSTEM_STATUS_OUT_OF_MEMORY_ERROR;
   }

   intptr_t address = iterator->WriteCursor;
   IncrementWriteCursor(iterator);

   return WriteBlock(handle, address, block);
}
