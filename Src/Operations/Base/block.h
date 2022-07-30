#pragma once

#include "../../CRC/crc.h"
#include "../../private.h"

#include "SDeviceCore/errors.h"

#define __IS_VALID_SECTOR_HEADER_STATE(state) (                                                                        \
{                                                                                                                      \
   __auto_type __state = (state);                                                                                      \
   __state == SECTOR_HEADER_STATE_ACTIVE           ||                                                                  \
   __state == SECTOR_HEADER_STATE_TRANSFER_ONGOING ||                                                                  \
   __state == SECTOR_HEADER_STATE_TRANSFER_END     ||                                                                  \
   __state == SECTOR_HEADER_STATE_ERASED;                                                                              \
})

#define __CEIL_INTEGER_DIVISION(numerator, denominator) (                                                              \
{                                                                                                                      \
   __auto_type __numerator = (numerator);                                                                              \
   __auto_type __denominator = (denominator);                                                                          \
   __numerator / __denominator + ((__numerator % __denominator != 0) ? 1 : 0);                                         \
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
   SECTOR_HEADER_STATE_ACTIVE           = 0x11111111,
   SECTOR_HEADER_STATE_TRANSFER_ONGOING = 0x55555555,
   SECTOR_HEADER_STATE_TRANSFER_END     = 0xAAAAAAAA,
   SECTOR_HEADER_STATE_ERASED           = 0xFFFFFFFF,
} SectorHeaderState;

typedef union
{
   struct __attribute__((packed))
   {
      union
      {
         struct __attribute__((packed))
         {
            CrcType Crc;
            uint8_t Data[sizeof(SimpleFsSDeviceBlockValue) - (sizeof(BlockDescriptor) + sizeof(CrcType))];
         } AsCommonService;

         struct __attribute__((packed))
         {
            CrcType Crc;
            SectorHeaderState State;
            uint8_t Padding;
         } AsHeader;

         struct __attribute__((packed))
         {
            CrcType Crc;
            bool IsDeleted;
            uint8_t FileSize;
            SimpleFsSDeviceAddress Address;
            uint8_t Padding;
         } AsDataPreamble;

         struct __attribute__((packed))
         {
            uint8_t Data[sizeof(SimpleFsSDeviceBlockValue) - sizeof(BlockDescriptor)];
         } AsData;
      };

      BlockDescriptor Descriptor;
   } AsBlock;

   SimpleFsSDeviceBlockValue AsValue;
   uint8_t AsBytes[sizeof(SimpleFsSDeviceBlockValue) / sizeof(uint8_t)];
} Block;

static inline uint8_t GetErasedByteValue(__SDEVICE_HANDLE(SimpleFs) *handle)
{
   return (handle->Init.IsErasingToZero == true) ? 0 : UINT8_MAX;
}

static inline SimpleFsSDeviceBlockValue GetErasedBlockValue(__SDEVICE_HANDLE(SimpleFs) *handle)
{
   return (handle->Init.IsErasingToZero == true) ? 0 : UINT64_MAX;
}

static inline bool IsBlockEmpty(__SDEVICE_HANDLE(SimpleFs) *handle, Block *block)
{
   return block->AsValue == GetErasedBlockValue(handle);
}

static inline bool IsBlockOfType(Block *block, BlockDescriptor type)
{
   return (block->AsBlock.Descriptor & type) == type;
}

static inline size_t GetBlocksSize(size_t blockCount)
{
   return blockCount * sizeof(Block);
}

static inline uintptr_t GetNextBlockAddress(uintptr_t address)
{
   return address + GetBlocksSize(1);
}

static inline uintptr_t GetPreviousBlockAddress(uintptr_t address)
{
   return address - GetBlocksSize(1);
}

static inline void ReadBlock(__SDEVICE_HANDLE(SimpleFs) *handle, uintptr_t address, Block *block)
{
   if(handle->Init.TryReadBlock(handle, address, &block->AsValue) != true)
      SDeviceThrow(handle, SIMPLE_FS_SDEVICE_EXCEPTION_READ_FAIL);
}

static inline void WriteBlock(__SDEVICE_HANDLE(SimpleFs) *handle, uintptr_t address, const Block *block)
{
   if(handle->Init.TryWriteBlock(handle, address, &block->AsValue) != true)
      SDeviceThrow(handle, SIMPLE_FS_SDEVICE_EXCEPTION_WRITE_FAIL);
}

static inline CrcType ComputeServiceBlockCrc(const Block *block)
{
   return SimpleFsSDeviceComputeCrc(block->AsBlock.AsCommonService.Data, sizeof(block->AsBlock.AsCommonService.Data));
}

static inline CrcType UpdateCrcWithDataBlock(const Block *block, CrcType crc, size_t size)
{
   SDeviceAssert(size <= sizeof(block->AsBlock.AsData.Data));
   return SimpleFsSDeviceUpdateCrc(block->AsBlock.AsData.Data, crc, size);
}

static inline size_t GetFileBlocksCount(size_t fileSize)
{
   return 1 + __CEIL_INTEGER_DIVISION(fileSize, GetBlocksSize(1) - sizeof(BlockDescriptor));
}
