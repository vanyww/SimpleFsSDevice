#pragma once

#include "sector.h"

#define __ACTIVE_ITERATOR(handle) (handle->Runtime.Iterators[handle->Runtime.ActiveIteratorIndex])
#define __ITERATOR_SECTOR(handle, iterator) (handle->Init.Sectors[iterator->SectorIndex])

static inline uintptr_t IteratorStart(__SDEVICE_HANDLE(FlashFileSystem) *handle, FlashFileSystemIterator *iterator)
{
   return SectorFirstBlockAddress(&__ITERATOR_SECTOR(handle, iterator));
}

static inline uintptr_t IteratorEnd(__SDEVICE_HANDLE(FlashFileSystem) *handle, FlashFileSystemIterator *iterator)
{
   return SectorLastBlockAddress(&__ITERATOR_SECTOR(handle, iterator));
}

static inline bool CanRead(__SDEVICE_HANDLE(FlashFileSystem) *handle, FlashFileSystemIterator *iterator)
{
   return iterator->ReadCursor >= IteratorStart(handle, iterator) &&
          iterator->ReadCursor <= IteratorEnd(handle, iterator);
}

static inline bool CanWrite(__SDEVICE_HANDLE(FlashFileSystem) *handle, FlashFileSystemIterator *iterator)
{
   return iterator->WriteCursor >= IteratorStart(handle, iterator) &&
          iterator->WriteCursor <= IteratorEnd(handle, iterator);
}

static inline void SeekReadCursor(FlashFileSystemIterator *iterator, uintptr_t offset)
{
   iterator->ReadCursor = offset;
}

static inline void SeekWriteCursor(FlashFileSystemIterator *iterator, uintptr_t offset)
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

static inline size_t EmptyBlocksCount(__SDEVICE_HANDLE(FlashFileSystem) *handle, FlashFileSystemIterator *iterator)
{
   if(CanWrite(handle, iterator) == false)
      return 0;

   return 1 + (SectorLastBlockAddress(&__ITERATOR_SECTOR(handle, iterator)) - iterator->WriteCursor) / BlocksSize(1);
}

static inline FlashFileSystemStatus PeekFromCurrentBlock(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                         FlashFileSystemIterator *iterator,
                                                         FileSystemBlock *block)
{
   if(CanRead(handle, iterator) != true)
      return FLASH_FILE_SYSTEM_STATUS_VALUE_NOT_FOUND_ERROR;

   return ReadBlock(handle, iterator->ReadCursor, block);
}

static inline FlashFileSystemStatus ReadForwardFromCurrentBlock(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                                FlashFileSystemIterator *iterator,
                                                                FileSystemBlock *block)
{
   if(CanRead(handle, iterator) != true)
      return FLASH_FILE_SYSTEM_STATUS_VALUE_NOT_FOUND_ERROR;

   uintptr_t address = iterator->ReadCursor;
   IncrementReadCursor(iterator);

   return ReadBlock(handle, address, block);
}

static inline FlashFileSystemStatus ReadBackwardFromCurrentBlock(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                                 FlashFileSystemIterator *iterator,
                                                                 FileSystemBlock *block)
{
   if(CanRead(handle, iterator) != true)
      return FLASH_FILE_SYSTEM_STATUS_VALUE_NOT_FOUND_ERROR;

   uintptr_t address = iterator->ReadCursor;
   DecrementReadCursor(iterator);

   return ReadBlock(handle, address, block);
}

static inline FlashFileSystemStatus WriteForwardToCurrentBlock(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                               FlashFileSystemIterator *iterator,
                                                               const FileSystemBlock *block)
{
   if(CanWrite(handle, iterator) != true)
   {
      SDeviceRuntimeErrorRaised(handle, FLASH_FILE_SYSTEM_RUNTIME_ERROR_OUT_OF_MEMORY);
      return FLASH_FILE_SYSTEM_STATUS_OUT_OF_MEMORY_ERROR;
   }

   uintptr_t address = iterator->WriteCursor;
   IncrementWriteCursor(iterator);

   return WriteBlock(handle, address, block);
}
