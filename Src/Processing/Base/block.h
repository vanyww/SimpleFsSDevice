#pragma once

#include "../../private.h"
#include "../../CRC/crc16.h"

#define __IS_VALID_BLOCK_HEADER_STATE(state) (                                                                         \
{                                                                                                                      \
   __typeof__(state) _state = state;                                                                                   \
   _state == HEADER_STATE_ACTIVE           ||                                                                          \
   _state == HEADER_STATE_TRANSFER_ONGOING ||                                                                          \
   _state == HEADER_STATE_TRANSFER_END     ||                                                                          \
   _state == HEADER_STATE_ERASED;                                                                                      \
})

#define __CEIL_INTEGER_DIVISION(numerator, denominator) (                                                              \
{                                                                                                                      \
   __typeof__(numerator) _numerator = numerator;                                                                       \
   __typeof__(denominator) _denominator = denominator;                                                                 \
   _numerator / _denominator + ((_numerator % _denominator != 0) ? 1 : 0);                                             \
})

typedef enum __attribute__((packed))
{
   BLOCK_DESCRIPTOR_HEADER        = 0x01,
   BLOCK_DESCRIPTOR_PREAMBLE      = 0x02,
   BLOCK_DESCRIPTOR_VARIABLE_DATA = 0x04,
   BLOCK_DESCRIPTOR_ERASED        = 0x80
} BlockDescriptor;

typedef enum __attribute__((packed))
{
   HEADER_STATE_ACTIVE           = 0x11111111,
   HEADER_STATE_TRANSFER_ONGOING = 0x55555555,
   HEADER_STATE_TRANSFER_END     = 0xAAAAAAAA,
   HEADER_STATE_ERASED           = 0xFFFFFFFF,
} BlockHeaderState;

typedef union
{
   struct __attribute__((packed)) CommonBlock
   {
      union
      {
         struct __attribute__((packed)) HeaderBlock
         {
            FlashFileSystemCrcType Crc;
            BlockHeaderState State;
            uint8_t Padding;
         } AsHeader;

         struct __attribute__((packed)) DataPreambleBlock
         {
            FlashFileSystemCrcType Crc;
            bool IsDeleted;
            uint8_t FileSize;
            FlashFileSystemAddress Address;
            uint8_t Padding;
         } AsDataPreamble;

         struct __attribute__((packed)) DataBlock
         {
            uint8_t Data[sizeof(uint64_t) - sizeof(BlockDescriptor)];
         } AsData;
      };

      BlockDescriptor Descriptor;
   } AsBlock;

   uint8_t AsBytes[sizeof(FlashFileSystemBlockValue) / sizeof(uint8_t)];
   uint16_t AsHalfWords[sizeof(FlashFileSystemBlockValue) / sizeof(uint16_t)];
   uint32_t AsWords[sizeof(FlashFileSystemBlockValue) / sizeof(uint32_t)];
   uint64_t AsDoubleWords[sizeof(FlashFileSystemBlockValue) / sizeof(uint64_t)];
   FlashFileSystemBlockValue AsValue;
} FileSystemBlock;

static inline uint8_t ErasedByteValue(__SDEVICE_HANDLE(FlashFileSystem) *handle)
{
   return (handle->Init.IsErasingToZero == true) ? 0 : UINT8_MAX;
}

static inline uint16_t ErasedHalfWordValue(__SDEVICE_HANDLE(FlashFileSystem) *handle)
{
   return (handle->Init.IsErasingToZero == true) ? 0 : UINT16_MAX;
}

static inline uint32_t ErasedWordValue(__SDEVICE_HANDLE(FlashFileSystem) *handle)
{
   return (handle->Init.IsErasingToZero == true) ? 0 : UINT32_MAX;
}

static inline uint64_t ErasedDoubleWordValue(__SDEVICE_HANDLE(FlashFileSystem) *handle)
{
   return (handle->Init.IsErasingToZero == true) ? 0 : UINT64_MAX;
}

static inline size_t BlocksSize(size_t blocksCount)
{
   return blocksCount * sizeof(FileSystemBlock);
}

static inline size_t FileBlocksCount(size_t variableSize)
{
   return 1 + __CEIL_INTEGER_DIVISION(variableSize, BlocksSize(1) - sizeof(BlockDescriptor));
}

static inline uintptr_t NextBlockAddress(uintptr_t address)
{
   return address + BlocksSize(1);
}

static inline uintptr_t PreviousBlockAddress(uintptr_t address)
{
   return address - BlocksSize(1);
}

static inline bool IsBlockEmpty(__SDEVICE_HANDLE(FlashFileSystem) *handle, FileSystemBlock *block)
{
   return block->AsDoubleWords[0] == ErasedDoubleWordValue(handle);
}

static inline bool IsBlockOfType(FileSystemBlock *block, BlockDescriptor type)
{
   return (block->AsBlock.Descriptor & type) == type;
}

static inline FlashFileSystemCrcType ComputeHeaderBlockCrc(FileSystemBlock *block)
{
   return FlashFileSystemComputeCrc16(&block->AsBlock.AsHeader.State,
                                      BlocksSize(1) - sizeof(FlashFileSystemCrcType));
}

static inline FlashFileSystemCrcType ComputeDataPreambleBlockCrc(FileSystemBlock *block)
{
   return FlashFileSystemComputeCrc16(&block->AsBlock.AsDataPreamble.IsDeleted,
                                      BlocksSize(1) - sizeof(FlashFileSystemCrcType));
}

static inline FlashFileSystemCrcType UpdateCrcWithDataBlock(FileSystemBlock *block,
                                                            FlashFileSystemCrcType crc,
                                                            size_t size)
{
   return FlashFileSystemUpdateCrc16(&block->AsBlock.AsData.Data,
                                     __MIN(size, sizeof(block->AsBlock.AsData.Data)),
                                     crc);
}

static inline FlashFileSystemStatus ReadBlock(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                              uintptr_t address,
                                              FileSystemBlock *block)
{
   if(handle->Init.TryReadBlock(handle, address, &block->AsValue) != true)
   {
      SDeviceRuntimeErrorRaised(handle, FLASH_FILE_SYSTEM_RUNTIME_ERROR_READ_FAIL);
      return FLASH_FILE_SYSTEM_STATUS_IO_MEMORY_ERROR;
   }

   return FLASH_FILE_SYSTEM_STATUS_OK;
}

static inline FlashFileSystemStatus WriteBlock(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                               uintptr_t address,
                                               const FileSystemBlock *block)
{
   if(handle->Init.TryWriteBlock(handle, address, &block->AsValue) != true)
   {
      SDeviceRuntimeErrorRaised(handle, FLASH_FILE_SYSTEM_RUNTIME_ERROR_WRITE_FAIL);
      return FLASH_FILE_SYSTEM_STATUS_IO_MEMORY_ERROR;
   }

   return FLASH_FILE_SYSTEM_STATUS_OK;
}
