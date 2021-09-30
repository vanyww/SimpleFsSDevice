#pragma once

#include "../../../Inc/FlashFileSystemSDevice/core.h"
#include "../../CRC/crc16.h"

#define __BYTE_EMPTY_VALUE UINT8_MAX
#define __BLOCK_EMPTY_VALUE UINT64_MAX
#define __CEIL_INTEGER_DIVISION(numerator, denominator)                                                                \
({                                                                                                                     \
   __typeof__(numerator) _numerator = numerator;                                                                       \
   __typeof__(denominator) _denominator = denominator;                                                                 \
   _numerator / _denominator + _numerator % _denominator;                                                              \
})

typedef enum __attribute__((packed))
{
   BLOCK_DESCRIPTOR_HEADER        = 0x01,
   BLOCK_DESCRIPTOR_DATA_PREAMBLE = 0x02,
   BLOCK_DESCRIPTOR_VARIABLE_DATA = 0x04,
   BLOCK_DESCRIPTOR_ERASED        = 0xFF
} BlockDescriptor;

typedef enum __attribute__((packed))
{
   HEADER_STATE_ACTIVE               = 0x11111111,
   HEADER_STATE_TRANSFER_IN_PROGRESS = 0x55555555,
   HEADER_STATE_TRANSFER_END         = 0xAAAAAAAA,
   HEADER_STATE_ERASED               = 0xFFFFFFFF,
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
            FlashFileSystemSDeviceAddress Address;
            uint8_t Padding;
         } AsDataPreamble;

         struct __attribute__((packed)) DataBlock
         {
            uint8_t Data[__FLASH_FILE_SYSTEM_SDEVICE_BLOCK_SIZE - sizeof(BlockDescriptor)];
         } AsData;
      };

      BlockDescriptor Descriptor;
   } AsBlock;

   uint8_t AsBytes[__FLASH_FILE_SYSTEM_SDEVICE_BLOCK_SIZE / sizeof(uint8_t)];
   uint16_t AsHalfWords[__FLASH_FILE_SYSTEM_SDEVICE_BLOCK_SIZE / sizeof(uint16_t)];
   uint32_t AsWords[__FLASH_FILE_SYSTEM_SDEVICE_BLOCK_SIZE / sizeof(uint32_t)];
   uint64_t AsDoubleWords[__FLASH_FILE_SYSTEM_SDEVICE_BLOCK_SIZE / sizeof(uint64_t)];
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
   return block->AsDoubleWords[0] == __BLOCK_EMPTY_VALUE;
}

static inline bool IsBlockOfType(FileSystemBlock *block, BlockDescriptor type)
{
   return (block->AsBlock.Descriptor & type) != 0;
}

static inline CrcType ComputeHeaderBlockCrc(FileSystemBlock *block)
{
   return Crc16Compute(&block->AsBlock.AsHeader.State, BlocksSize(1) - sizeof(CrcType));
}

static inline CrcType ComputeDataPreambleBlockCrc(FileSystemBlock *block)
{
   return Crc16Compute(&block->AsBlock.AsDataPreamble.IsDeleted, BlocksSize(1) - sizeof(CrcType));
}

static inline CrcType UpdateCrcByDataBlock(FileSystemBlock *block, CrcType crc)
{
   return Crc16Update(&block->AsBlock.AsData.Data, BlocksSize(1) - sizeof(BlockDescriptor), crc);
}
