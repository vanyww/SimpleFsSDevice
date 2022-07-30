#include "operations.h"
#include "Base/iterator.h"
#include "Helpers/helpers.h"

#include <memory.h>

bool SimpleFsSDeviceTryWriteFile(__SDEVICE_HANDLE(SimpleFs) *handle,
                                 SimpleFsSDeviceAddress address,
                                 const void *data,
                                 size_t size,
                                 bool delete)
{
   size_t fileBlocksCount = GetFileBlocksCount(size);

   /* check if there is enough free space and initiate transfer if not */
   if(GetIteratorEmptyBlocksCount(handle->Runtime.ActiveIterator) < fileBlocksCount)
   {
      SimpleFsSDeviceTransferSectors(handle);

      /* if there is still not enough space after transfer */
      if(GetIteratorEmptyBlocksCount(handle->Runtime.ActiveIterator) < fileBlocksCount)
         return false;
   }

   /* create preamble block */
   Block preambleBlock =
   {
      .AsBlock =
      {
         .Descriptor = BLOCK_DESCRIPTOR_PREAMBLE,
         .AsDataPreamble =
         {
            .IsDeleted = delete,
            .FileSize = size,
            .Address = address,
            .Padding = GetErasedByteValue(handle)
         }
      }
   };

   CrcType crc = ComputeServiceBlockCrc(&preambleBlock);
   preambleBlock.AsBlock.AsDataPreamble.Crc = SimpleFsSDeviceUpdateCrc(data, size, crc);

   /* write preamble block */
   IteratorTryWriteForward(handle, handle->Runtime.ActiveIterator, &preambleBlock);

   /* compose and write data blocks */
   while(size > 0)
   {
      Block dataBlock = { .AsBlock.Descriptor = BLOCK_DESCRIPTOR_VARIABLE_DATA };
      size_t blockWriteSize = __MIN(size, sizeof(dataBlock.AsBlock.AsData.Data));
      memcpy(dataBlock.AsBlock.AsData.Data, data, blockWriteSize);

      /* set all unused data bytes to empty value */
      memset(&dataBlock.AsBlock.AsData.Data[blockWriteSize],
             GetErasedByteValue(handle),
             sizeof(dataBlock.AsBlock.AsData.Data) - blockWriteSize);

      IteratorTryWriteForward(handle, handle->Runtime.ActiveIterator, &dataBlock);

      data += blockWriteSize;
      size -= blockWriteSize;
   }

   return true;
}

void SimpleFsSDeviceProcessInitialState(__SDEVICE_HANDLE(SimpleFs) *handle)
{
   SectorHeaderState sectorHeaderStates[__SIMPLE_FS_SDEVICE_SECTORS_COUNT];

   /* read and partially preprocess sectors initial states */
   for(size_t i = 0; i < __SIMPLE_FS_SDEVICE_SECTORS_COUNT; i++)
   {
      Iterator *iterator = &handle->Runtime.Iterators[i];

      /* find first written block starting from end of sector */
      SeekIteratorReadCursor(iterator, GetIteratorEndAddress(iterator));
      if(IteratorTryReadBackToNotEmptyBlock(handle, iterator) != true)
      {
         /* sector is empty, write header with erased state */
         SeekIteratorWriteCursor(iterator, GetIteratorStartAddress(iterator));
         SimpleFsSDeviceSetSectorHeaderState(handle, iterator, SECTOR_HEADER_STATE_ERASED);
         sectorHeaderStates[i] = SECTOR_HEADER_STATE_ERASED;

         continue;
      }

      SeekIteratorWriteCursor(iterator, GetNextBlockAddress(iterator->ReadCursor));

      /* if sector has no valid header, format it to erased state */
      if(SimpleFsSDeviceTryGetSectorHeaderState(handle, iterator, &sectorHeaderStates[i]) != true)
      {
         SimpleFsSDeviceFormatSectorToState(handle, &handle->Runtime.Iterators[i], SECTOR_HEADER_STATE_ERASED);
         sectorHeaderStates[i] = SECTOR_HEADER_STATE_ERASED;
      }
   }

   /* process sector header block state combinations */
   switch(sectorHeaderStates[0])
   {
      case SECTOR_HEADER_STATE_ACTIVE:
         switch(sectorHeaderStates[1])
         {
            case SECTOR_HEADER_STATE_TRANSFER_ONGOING:
               SimpleFsSDeviceFormatSectorToState(handle, &handle->Runtime.Iterators[1], SECTOR_HEADER_STATE_ERASED);
               handle->Runtime.ActiveIterator = &handle->Runtime.Iterators[0];
               SimpleFsSDeviceTransferSectors(handle);
               break;

            case SECTOR_HEADER_STATE_TRANSFER_END:
               SimpleFsSDeviceFormatSectorToState(handle, &handle->Runtime.Iterators[0], SECTOR_HEADER_STATE_ERASED);
               handle->Runtime.ActiveIterator = &handle->Runtime.Iterators[1];
               SimpleFsSDeviceFormatSectorToState(handle, handle->Runtime.ActiveIterator, SECTOR_HEADER_STATE_ACTIVE);
               break;

            case SECTOR_HEADER_STATE_ERASED:
               handle->Runtime.ActiveIterator = &handle->Runtime.Iterators[0];
               break;

            /* invalid state */
            case SECTOR_HEADER_STATE_ACTIVE:
               SimpleFsSDeviceClearMemoryState(handle);
               break;

            default:
               SDeviceAssert(false);
         }
         break;

      case SECTOR_HEADER_STATE_TRANSFER_ONGOING:
         switch(sectorHeaderStates[1])
         {
            case SECTOR_HEADER_STATE_ACTIVE:
               SimpleFsSDeviceFormatSectorToState(handle, &handle->Runtime.Iterators[0], SECTOR_HEADER_STATE_ERASED);
               handle->Runtime.ActiveIterator = &handle->Runtime.Iterators[1];
               SimpleFsSDeviceTransferSectors(handle);
               break;

            /* invalid states */
            case SECTOR_HEADER_STATE_TRANSFER_ONGOING:
               /* fall through */
            case SECTOR_HEADER_STATE_TRANSFER_END:
               /* fall through */
            case SECTOR_HEADER_STATE_ERASED:
               SimpleFsSDeviceClearMemoryState(handle);
               break;

            default:
               SDeviceAssert(false);
         }
         break;

      case SECTOR_HEADER_STATE_TRANSFER_END:
         switch(sectorHeaderStates[1])
         {
            case SECTOR_HEADER_STATE_ACTIVE:
               SimpleFsSDeviceFormatSectorToState(handle, &handle->Runtime.Iterators[1], SECTOR_HEADER_STATE_ERASED);
               /* fall through */
            case SECTOR_HEADER_STATE_ERASED:
               handle->Runtime.ActiveIterator = &handle->Runtime.Iterators[0];
               SimpleFsSDeviceFormatSectorToState(handle, handle->Runtime.ActiveIterator, SECTOR_HEADER_STATE_ACTIVE);
               break;

            /* invalid states */
            case SECTOR_HEADER_STATE_TRANSFER_ONGOING:
               /* fall through */
            case SECTOR_HEADER_STATE_TRANSFER_END:
               SimpleFsSDeviceClearMemoryState(handle);

            default:
               SDeviceAssert(false);
         }
         break;

      case SECTOR_HEADER_STATE_ERASED:
         switch(sectorHeaderStates[1])
         {
            case SECTOR_HEADER_STATE_ACTIVE:
               handle->Runtime.ActiveIterator = &handle->Runtime.Iterators[1];
               break;

            case SECTOR_HEADER_STATE_TRANSFER_END:
               /* fall through */
            case SECTOR_HEADER_STATE_ERASED:
               handle->Runtime.ActiveIterator = &handle->Runtime.Iterators[1];
               SimpleFsSDeviceFormatSectorToState(handle, handle->Runtime.ActiveIterator, SECTOR_HEADER_STATE_ACTIVE);
               break;

            /* invalid states */
            case SECTOR_HEADER_STATE_TRANSFER_ONGOING:
               SimpleFsSDeviceClearMemoryState(handle);

            default:
               SDeviceAssert(false);
         }
         break;

      default:
         SDeviceAssert(false);
   }
}

