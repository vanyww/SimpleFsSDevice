#pragma once

#include "../IO/Selection/Filters/ExceptEmpty/filter.h"
#include "File/file_read.h"
#include "File/file_write.h"
#include "format.h"

static SectorState PreprocessInitialStreamState(ThisHandle *handle, Stream *stream)
{
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
   SectorState mainSectorState =
         PreprocessInitialStreamState(handle, &handle->Runtime->MainSectorWriteStream);

   SectorState auxiliarySectorState =
         PreprocessInitialStreamState(handle, &handle->Runtime->AuxiliarySectorWriteStream);

   switch(mainSectorState)
   {
      case SECTOR_STATE_ACTIVE:
         switch(auxiliarySectorState)
         {
            case SECTOR_STATE_TRANSFER_ONGOING:
               FormatStreamSector(handle, &handle->Runtime->AuxiliarySectorWriteStream);

               /* fall through */
            case SECTOR_STATE_ERASED:
               SetActiveWriteStream(handle, &handle->Runtime->MainSectorWriteStream);
               SetInactiveWriteStream(handle, &handle->Runtime->AuxiliarySectorWriteStream);
               return;

            case SECTOR_STATE_TRANSFER_END:
               FormatStreamSector(handle, &handle->Runtime->MainSectorWriteStream);
               WriteStreamSectorState(handle, &handle->Runtime->AuxiliarySectorWriteStream, SECTOR_STATE_ACTIVE);
               SetActiveWriteStream(handle, &handle->Runtime->AuxiliarySectorWriteStream);
               SetInactiveWriteStream(handle, &handle->Runtime->MainSectorWriteStream);
               return;

            default:
               break;
         }
         break;

      case SECTOR_STATE_TRANSFER_ONGOING:
         switch(auxiliarySectorState)
         {
            case SECTOR_STATE_ACTIVE:
               FormatStreamSector(handle, &handle->Runtime->MainSectorWriteStream);
               SetActiveWriteStream(handle, &handle->Runtime->AuxiliarySectorWriteStream);
               SetInactiveWriteStream(handle, &handle->Runtime->MainSectorWriteStream);
               return;

            default:
               break;
         }
         break;

      case SECTOR_STATE_TRANSFER_END:
         switch(auxiliarySectorState)
         {
            case SECTOR_STATE_ACTIVE:
               FormatStreamSector(handle, &handle->Runtime->AuxiliarySectorWriteStream);

               /* fall through */
            case SECTOR_STATE_ERASED:
               WriteStreamSectorState(handle, &handle->Runtime->MainSectorWriteStream, SECTOR_STATE_ACTIVE);
               SetActiveWriteStream(handle, &handle->Runtime->MainSectorWriteStream);
               SetInactiveWriteStream(handle, &handle->Runtime->AuxiliarySectorWriteStream);
               return;

            default:
               break;
         }
         break;

      case SECTOR_STATE_ERASED:
         switch(auxiliarySectorState)
         {
            case SECTOR_STATE_TRANSFER_END:

               /* fall through */
            case SECTOR_STATE_ERASED:
               WriteStreamSectorState(handle, &handle->Runtime->AuxiliarySectorWriteStream, SECTOR_STATE_ACTIVE);

               /* fall through */
            case SECTOR_STATE_ACTIVE:
               SetActiveWriteStream(handle, &handle->Runtime->AuxiliarySectorWriteStream);
               SetInactiveWriteStream(handle, &handle->Runtime->MainSectorWriteStream);
               return;

            default:
               break;
         }
         break;
   }

   ForceReformatMemory(handle);
}
