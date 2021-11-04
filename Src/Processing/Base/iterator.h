#pragma once

#include "sector.h"

static inline intptr_t IteratorStart(FlashFileSystemSDeviceIterator *iterator)
{
   return SectorFirstBlockAddress(iterator->Sector);
}

static inline intptr_t IteratorEnd(FlashFileSystemSDeviceIterator *iterator)
{
   return SectorLastBlockAddress(iterator->Sector);
}

static inline bool CanRead(FlashFileSystemSDeviceIterator *iterator)
{
   return iterator->ReadCursor >= IteratorStart(iterator) &&
          iterator->ReadCursor <= IteratorEnd(iterator);
}

static inline bool CanWrite(FlashFileSystemSDeviceIterator *iterator)
{
   return iterator->WriteCursor >= IteratorStart(iterator) &&
          iterator->WriteCursor <= IteratorEnd(iterator);
}

static inline void SeekReadCursor(FlashFileSystemSDeviceIterator *iterator, intptr_t offset)
{
   iterator->ReadCursor = offset;
}

static inline void SeekWriteCursor(FlashFileSystemSDeviceIterator *iterator, intptr_t offset)
{
   iterator->WriteCursor = offset;
}

static inline void IncrementReadCursor(FlashFileSystemSDeviceIterator *iterator)
{
   SeekReadCursor(iterator, NextBlockAddress(iterator->ReadCursor));
}

static inline void DecrementReadCursor(FlashFileSystemSDeviceIterator *iterator)
{
   SeekReadCursor(iterator, PreviousBlockAddress(iterator->ReadCursor));
}

static inline void IncrementWriteCursor(FlashFileSystemSDeviceIterator *iterator)
{
   SeekWriteCursor(iterator, NextBlockAddress(iterator->WriteCursor));
}

static inline void DecrementWriteCursor(FlashFileSystemSDeviceIterator *iterator)
{
   SeekWriteCursor(iterator, PreviousBlockAddress(iterator->WriteCursor));
}

static inline size_t EmptyBlocksCount(FlashFileSystemSDeviceIterator *iterator)
{
   if(CanWrite(iterator) == false)
      return 0;

   return 1 + (SectorLastBlockAddress(iterator->Sector) - iterator->WriteCursor) / BlocksSize(1);
}

static inline FlashFileSystemSDeviceState PeekFromCurrentBlock(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                               FlashFileSystemSDeviceIterator *iterator,
                                                               FileSystemBlock *block)
{
   if(CanRead(iterator) != true)
      return FLASH_FILE_SYSTEM_SDEVICE_STATE_VALUE_NOT_FOUND_ERROR;

   return ReadBlock(handle, iterator->ReadCursor, block);
}

static inline FlashFileSystemSDeviceState ReadForwardFromCurrentBlock(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                                      FlashFileSystemSDeviceIterator *iterator,
                                                                      FileSystemBlock *block)
{
   if(CanRead(iterator) != true)
      return FLASH_FILE_SYSTEM_SDEVICE_STATE_VALUE_NOT_FOUND_ERROR;

   intptr_t address = iterator->ReadCursor;
   IncrementReadCursor(iterator);
   return ReadBlock(handle, address, block);
}

static inline FlashFileSystemSDeviceState ReadBackwardFromCurrentBlock(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                                       FlashFileSystemSDeviceIterator *iterator,
                                                                       FileSystemBlock *block)
{
   if(CanRead(iterator) != true)
      return FLASH_FILE_SYSTEM_SDEVICE_STATE_VALUE_NOT_FOUND_ERROR;

   intptr_t address = iterator->ReadCursor;
   DecrementReadCursor(iterator);
   return ReadBlock(handle, address, block);
}

static inline FlashFileSystemSDeviceState WriteForwardToCurrentBlock(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                                                     FlashFileSystemSDeviceIterator *iterator,
                                                                     const FileSystemBlock *block)
{
   if(CanWrite(iterator) != true)
   {
      SDeviceRuntimeErrorRaised(handle, FLASH_FILE_SYSTEM_SDEVICE_RUNTIME_OUT_OF_MEMORY_ERROR);
      return FLASH_FILE_SYSTEM_SDEVICE_STATE_OUT_OF_MEMORY_ERROR;
   }

   intptr_t address = iterator->WriteCursor;
   IncrementWriteCursor(iterator);
   return WriteBlock(handle, address, block);
}
