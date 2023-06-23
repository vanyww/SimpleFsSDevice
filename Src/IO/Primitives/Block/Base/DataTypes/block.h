#pragma once

#include "Enumerations/sector_state.h"
#include "Enumerations/block_type.h"

#include "SDeviceCore/core.h"

#include <assert.h>

/* normal blocks ******************************************************************************************************/

typedef struct __attribute__((packed))
{
   uint8_t BlockCrc;
   uint8_t FileAreaLength;
   uint8_t LastFileBlockDataSize;
   uint16_t FileId;
   uint16_t FileCrc;
   BlockType Type;
} FileAreaTagBlock;

typedef struct __attribute__((packed))
{
   uint8_t BlockCrc;
   uint8_t BadAreaLength;
   uint8_t Padding[sizeof(uint64_t) - (sizeof(uint8_t) + sizeof(uint8_t) + sizeof(BlockType))];
   BlockType Type;
} BadAreaTagBlock;

typedef struct __attribute__((packed))
{
   uint8_t BlockCrc;
   SectorState SectorState;
   SDeviceVersion FsVersion;
   uint8_t Padding;
   BlockType Type;
} HeaderBlock;

typedef struct __attribute__((packed))
{
   uint8_t Data[sizeof(uint64_t) - sizeof(BlockType)];
   BlockType Type;
} FileDataBlock;

/* abstract blocks ****************************************************************************************************/

typedef union __attribute__((transparent_union))
{
   struct __attribute__((packed))
   {
      uint8_t BlockCrc;
      uint8_t AreaLength;
      uint8_t BlockData[sizeof(uint64_t) - (sizeof(uint8_t) + sizeof(uint8_t))];
   };

   FileAreaTagBlock AsFileAreaTag;
   BadAreaTagBlock AsBadAreaTag;
} AreaTagBlock;

typedef union __attribute__((transparent_union))
{
   struct __attribute__((packed))
   {
      uint8_t BlockCrc;
      uint8_t BlockData[sizeof(uint64_t) - sizeof(uint8_t)];
   };

   FileAreaTagBlock AsFileAreaTag;
   BadAreaTagBlock AsBadAreaTag;
   HeaderBlock AsHeader;

   AreaTagBlock AsAreaTag;
} ServiceBlock;

typedef union __attribute__((transparent_union))
{
   struct __attribute__((packed))
   {
      uint8_t BlockData[sizeof(uint64_t) - sizeof(BlockType)];
      BlockType Type;
   };

   FileAreaTagBlock AsFileAreaTag;
   BadAreaTagBlock AsBadAreaTag;
   FileDataBlock AsFileData;
   HeaderBlock AsHeader;

   AreaTagBlock AsAreaTag;
   ServiceBlock AsService;

   uint64_t AsValue;
} Block;

/**********************************************************************************************************************/

static_assert(sizeof(Block) == sizeof(uint64_t), "SimpleFsSDevice block has incorrect size (must be sizeof(uint64_t))");
