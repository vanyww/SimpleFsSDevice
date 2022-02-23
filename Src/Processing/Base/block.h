#pragma once

#include "../../../Inc/FlashFileSystemSDevice/core.h"
#include "../../CRC/crc16.h"

#define __BYTE_EMPTY_VALUE        UINT8_MAX
#define __HALF_WORD_EMPTY_VALUE   UINT16_MAX
#define __WORD_EMPTY_VALUE        UINT32_MAX
#define __DOUBLE_WORD_EMPTY_VALUE UINT64_MAX

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
   BLOCK_DESCRIPTOR_ERASED        = __BYTE_EMPTY_VALUE
} BlockDescriptor;

typedef enum __attribute__((packed))
{
   HEADER_STATE_ACTIVE           = 0x11111111,
   HEADER_STATE_TRANSFER_ONGOING = 0x55555555,
   HEADER_STATE_TRANSFER_END     = 0xAAAAAAAA,
   HEADER_STATE_ERASED           = __WORD_EMPTY_VALUE,
} BlockHeaderState;

typedef union
{
   struct __attribute__((packed)) CommonBlock
   {
      union
      {
         struct __attribute__((packed)) HeaderBlock
         {
            CrcType Crc;
            BlockHeaderState State;
            uint8_t Padding;
         } AsHeader;

         struct __attribute__((packed)) DataPreambleBlock
         {
            CrcType Crc;
            bool IsDeleted;
            uint8_t VariableSize;
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

   uint8_t AsBytes[sizeof(uint64_t) / sizeof(uint8_t)];
   uint16_t AsHalfWords[sizeof(uint64_t) / sizeof(uint16_t)];
   uint32_t AsWords[sizeof(uint64_t) / sizeof(uint32_t)];
   uint64_t AsDoubleWords[sizeof(uint64_t) / sizeof(uint64_t)];
} FileSystemBlock;

static inline size_t BlocksSize(size_t blocksCount)
{
   return blocksCount * sizeof(FileSystemBlock);
}

static inline size_t VariableBlocksCount(size_t variableSize)
{
   return 1 + __CEIL_INTEGER_DIVISION(variableSize, BlocksSize(1) - sizeof(BlockDescriptor));
}

static inline intptr_t NextBlockAddress(intptr_t address)
{
   return address + BlocksSize(1);
}

static inline intptr_t PreviousBlockAddress(intptr_t address)
{
   return address - BlocksSize(1);
}

static inline bool IsBlockEmpty(FileSystemBlock *block)
{
   return block->AsDoubleWords[0] == __DOUBLE_WORD_EMPTY_VALUE;
}

static inline bool IsBlockOfType(FileSystemBlock *block, BlockDescriptor type)
{
   return (block->AsBlock.Descriptor & type) == type;
}

static inline CrcType ComputeHeaderBlockCrc(FileSystemBlock *block)
{
   return ComputeCrc16(&block->AsBlock.AsHeader.State, BlocksSize(1) - sizeof(CrcType));
}

static inline CrcType ComputeDataPreambleBlockCrc(FileSystemBlock *block)
{
   return ComputeCrc16(&block->AsBlock.AsDataPreamble.IsDeleted, BlocksSize(1) - sizeof(CrcType));
}

static inline CrcType UpdateCrcWithDataBlock(FileSystemBlock *block, CrcType crc, size_t size)
{
   return UpdateCrc16(&block->AsBlock.AsData.Data, __MIN(size, sizeof(block->AsBlock.AsData.Data)), crc);
}

static inline FlashFileSystemStatus ReadBlock(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                              intptr_t address,
                                              FileSystemBlock *block)
{
   if(handle->Init.TryRead(handle, address, BlocksSize(1), block) != true)
   {
      SDeviceRuntimeErrorRaised(handle, FLASH_FILE_SYSTEM_RUNTIME_ERROR_READ_FAIL);
      return FLASH_FILE_SYSTEM_STATUS_IO_MEMORY_ERROR;
   }

   return FLASH_FILE_SYSTEM_STATUS_OK;
}

static inline FlashFileSystemStatus WriteBlock(__SDEVICE_HANDLE(FlashFileSystem) *handle,
                                               intptr_t address,
                                               const FileSystemBlock *block)
{
   if(handle->Init.TryWrite(handle, address, BlocksSize(1), block) != true)
   {
      SDeviceRuntimeErrorRaised(handle, FLASH_FILE_SYSTEM_RUNTIME_ERROR_WRITE_FAIL);
      return FLASH_FILE_SYSTEM_STATUS_IO_MEMORY_ERROR;
   }

   return FLASH_FILE_SYSTEM_STATUS_OK;
}
