#pragma once

#include "../IO/Selection/Filters/ExceptEmpty/filter.h"
#include "File/file_read.h"
#include "File/file_write.h"
#include "format.h"

static SectorState PreprocessInitialStreamState(ThisHandle *handle, Stream *stream)
{
   SDeviceDebugAssert(handle != NULL);
   SDeviceDebugAssert(stream != NULL);

   const SelectionFilter filters[] = { COMPOSE_SELECTION_FILTER(ExceptEmpty) };
   BlockSelector selector = CreateBlockSelector(handle, filters, LENGTHOF(filters));

   SeekStream(stream, SEEK_STREAM_ORIGIN_ENDING, 0);

   Block readBlock;
   if(!TrySelectNextStreamBlock(handle, stream, &selector, &readBlock))
   {
      SeekStream(stream, SEEK_STREAM_ORIGIN_BEGINNING, 0);
      WriteStreamSectorState(handle, stream, SECTOR_STATE_ERASED);
      return SECTOR_STATE_ERASED;
   }

   SectorState state;
   ReadStream readStream = CloneStream(stream);
   if(!TryReadStreamSectorState(handle, &readStream, &state))
   {
      FormatStreamSector(handle, stream);
      return SECTOR_STATE_ERASED;
   }

   SeekStream(stream, SEEK_STREAM_ORIGIN_CURRENT, +1);
   return state;
}

static void ProcessInitialMemoryState(ThisHandle *handle)
{
   SDeviceDebugAssert(handle != NULL);

   SectorState sector$0State = PreprocessInitialStreamState(handle, &handle->Runtime.Sector$0WriteStream);
   SectorState sector$1State = PreprocessInitialStreamState(handle, &handle->Runtime.Sector$1WriteStream);

   switch(sector$0State)
   {
      case SECTOR_STATE_ACTIVE:
         switch(sector$1State)
         {
            case SECTOR_STATE_TRANSFER_ONGOING:
               FormatStreamSector(handle, &handle->Runtime.Sector$1WriteStream);

               /* fall through */
            case SECTOR_STATE_ERASED:
               SetActiveWriteStream(handle, &handle->Runtime.Sector$0WriteStream);
               SetInactiveWriteStream(handle, &handle->Runtime.Sector$1WriteStream);
               return;

            case SECTOR_STATE_TRANSFER_END:
               FormatStreamSector(handle, &handle->Runtime.Sector$0WriteStream);
               WriteStreamSectorState(handle, &handle->Runtime.Sector$1WriteStream, SECTOR_STATE_ACTIVE);
               SetActiveWriteStream(handle, &handle->Runtime.Sector$1WriteStream);
               SetInactiveWriteStream(handle, &handle->Runtime.Sector$0WriteStream);
               return;

            default:
               break;
         }
         break;

      case SECTOR_STATE_TRANSFER_ONGOING:
         switch(sector$1State)
         {
            case SECTOR_STATE_ACTIVE:
               FormatStreamSector(handle, &handle->Runtime.Sector$0WriteStream);
               SetActiveWriteStream(handle, &handle->Runtime.Sector$1WriteStream);
               SetInactiveWriteStream(handle, &handle->Runtime.Sector$0WriteStream);
               return;

            default:
               break;
         }
         break;

      case SECTOR_STATE_TRANSFER_END:
         switch(sector$1State)
         {
            case SECTOR_STATE_ACTIVE:
               FormatStreamSector(handle, &handle->Runtime.Sector$1WriteStream);

               /* fall through */
            case SECTOR_STATE_ERASED:
               WriteStreamSectorState(handle, &handle->Runtime.Sector$0WriteStream, SECTOR_STATE_ACTIVE);
               SetActiveWriteStream(handle, &handle->Runtime.Sector$0WriteStream);
               SetInactiveWriteStream(handle, &handle->Runtime.Sector$1WriteStream);
               return;

            default:
               break;
         }
         break;

      case SECTOR_STATE_ERASED:
         switch(sector$1State)
         {
            case SECTOR_STATE_TRANSFER_END:

               /* fall through */
            case SECTOR_STATE_ERASED:
               WriteStreamSectorState(handle, &handle->Runtime.Sector$1WriteStream, SECTOR_STATE_ACTIVE);

               /* fall through */
            case SECTOR_STATE_ACTIVE:
               SetActiveWriteStream(handle, &handle->Runtime.Sector$1WriteStream);
               SetInactiveWriteStream(handle, &handle->Runtime.Sector$0WriteStream);
               return;

            default:
               break;
         }
         break;
   }

   ForceReformatMemory(handle);
}
