#pragma once

#include "Common/bit_operations.h"
#include "File/file_write.h"
#include "File/file_read.h"
#include "format.h"

typedef struct
{
   uint16_t    FileIdx;
   const void *Data;
   size_t      Size;
} TransferWriteFileInfo;

typedef struct
{
   bool     DoExclude;
   uint16_t FileIdx;
} CopyFileExclusion;

static void CopyStreamFiles(ThisHandle *handle, ReadStream *from, WriteStream *to, CopyFileExclusion exclusion)
{
   FileIdsRange idsRange;
   ReadStream idsRangeReadStream = CloneStream(from);
   if(!TryReadStreamFileIdsRange(handle, &idsRangeReadStream, &idsRange))
      return;

   uint32_t transferedIdsBits;
   for(uint32_t idOffset = idsRange.Lowest; idOffset <= idsRange.Highest; idOffset += BIT_SIZEOF(transferedIdsBits))
   {
      const SelectionFilter filters[] = { COMPOSE_SELECTION_FILTER(ValidServiceOfType, BLOCK_TYPE_FILE_AREA_TAG) };
      BlockSelector selector = CreateBlockSelector(handle, filters, LENGTHOF(filters));
      ReadStream fileStream = CloneStream(from);
      transferedIdsBits = 0;
      Block readBlock;

      while(TrySelectNextStreamBlock(handle, &fileStream, &selector, &readBlock))
      {
         FileAreaInfo fileInfo = BuildFileAreaInfo(readBlock.AsFileAreaTag);

         if(fileInfo.FileIdx < idOffset || fileInfo.FileIdx >= idOffset + BIT_SIZEOF(transferedIdsBits))
            continue;

         if(exclusion.DoExclude && exclusion.FileIdx == fileInfo.FileIdx)
            continue;

         uint32_t fileIdBit = fileInfo.FileIdx - idOffset;

         if(READ_NTH_BIT(transferedIdsBits, fileIdBit))
            continue;

         if(fileInfo.FileSize <= 0)
         {
            SET_NTH_BIT(transferedIdsBits, fileIdBit);
            continue;
         }

         uint8_t buffer[fileInfo.FileSize];
         FileAreaHandle areaHandle = CreateFileAreaHandle(&fileInfo, &fileStream);

         if(TryReadFileAreaData(handle, &areaHandle, buffer))
         {
            if(!TryWriteStreamFile(handle, to, fileInfo.FileIdx, buffer, fileInfo.FileSize))
               SDevicePanic(handle, SIMPLE_FS_SDEVICE_PANIC_OUT_OF_MEMORY);

            SET_NTH_BIT(transferedIdsBits, fileIdBit);

            if(COUNT_SET_BITS(transferedIdsBits) == MIN(idsRange.Highest - idOffset + 1, BIT_SIZEOF(transferedIdsBits)))
               break;
         }
      }
   }
}

static void TransferActiveStream(ThisHandle *handle, const TransferWriteFileInfo *fileInfo)
{
   ReadStream   sourceStream = BuildActiveReadStream(handle);
   WriteStream *targetStream = GetInactiveWriteStream(handle);

   WriteStreamSectorState(handle, targetStream, SECTOR_STATE_TRANSFER_ONGOING);

   CopyFileExclusion exclusion =
         (fileInfo) ? (CopyFileExclusion){ true, fileInfo->FileIdx } : (CopyFileExclusion){ false };
   CopyStreamFiles(handle, &sourceStream, targetStream, exclusion);

   if(fileInfo && fileInfo->Size > 0)
   {
      if(!TryWriteStreamFile(handle, targetStream, fileInfo->FileIdx, fileInfo->Data, fileInfo->Size))
         SDevicePanic(handle, SIMPLE_FS_SDEVICE_PANIC_OUT_OF_MEMORY);
   }

   WriteStreamSectorState(handle, targetStream, SECTOR_STATE_TRANSFER_END);
   FormatStreamSector(handle, GetActiveWriteStream(handle));
   WriteStreamSectorState(handle, targetStream, SECTOR_STATE_ACTIVE);
   SwitchWriteStreams(handle);
}
