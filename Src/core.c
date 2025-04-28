#include "Mid-layer/initialization.h"
#include "Mid-layer/integrity.h"

#include "SDeviceCore/heap.h"

SDEVICE_CREATE_HANDLE_DECLARATION(SimpleFs, init, context)
{
   SDeviceAssert(init);

   const ThisInitData *_init = init;

   SDeviceAssert(_init->ReadUInt64);
   SDeviceAssert(_init->WriteUInt64);
   SDeviceAssert(_init->EraseSector);

#if SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
   SDeviceAssert(_init->UpdateCrc8);
   SDeviceAssert(_init->ComputeCrc8);
   SDeviceAssert(_init->UpdateCrc16);
   SDeviceAssert(_init->ComputeCrc16);
#endif

   SDeviceAssert(HasSectorValidSize(&_init->PrimarySector));
   SDeviceAssert(HasSectorValidSize(&_init->AuxiliarySector));

   ThisHandle *instance = SDeviceAllocateHandle(sizeof(*instance->Init), sizeof(*instance->Runtime));

   instance->Context = context;
   *instance->Init = *_init;

   *instance->Runtime = (ThisRuntimeData)
   {
      .MainSectorWriteStream      = { .Sector = &instance->Init->PrimarySector,   .IsInBounds = false },
      .AuxiliarySectorWriteStream = { .Sector = &instance->Init->AuxiliarySector, .IsInBounds = false },
      .InactiveWriteStream        = NULL,
      .ActiveWriteStream          = NULL
   };

   InitializeCrc8();
   InitializeCrc16();

   ProcessInitialMemoryState(instance);

   return instance;
}

SDEVICE_DISPOSE_HANDLE_DECLARATION(SimpleFs, handlePointer)
{
   SDeviceAssert(handlePointer);

   ThisHandle **_handlePointer = handlePointer;
   ThisHandle *handle = *_handlePointer;

   SDeviceAssert(handle);

   SDeviceFreeHandle(handle);

   *_handlePointer = NULL;
}

SDEVICE_GET_SIMPLE_PROPERTY_DECLARATION(SimpleFs, TotalBadBlocksCount, handle, value)
{
   SDeviceAssert(handle);

   SDeviceAssert(value);

   size_t totalBadBlocksCount = ComputeTotalBadBlocksCount(handle);
   memcpy(value, &totalBadBlocksCount, sizeof(totalBadBlocksCount));

   return SDEVICE_PROPERTY_STATUS_OK;
}

void SimpleFsSDeviceFormatMemory(ThisHandle *handle)
{
   SDeviceAssert(handle);

   FormatMemory(handle);
}

void SimpleFsSDeviceForceHistoryDeletion(ThisHandle *handle)
{
   SDeviceAssert(handle);

   TransferActiveStream(handle, NULL);
}

void SimpleFsSDeviceWriteFile(ThisHandle *handle, uint16_t fileIdx, const void *data, size_t size)
{
   SDeviceAssert(handle);

   SDeviceAssert(data);

   SDeviceAssert(size > 0 && size <= MAX_FILE_SIZE);

   if(!TryWriteStreamFile(handle, GetActiveWriteStream(handle), fileIdx, data, size))
   {
      TransferWriteFileInfo transferFileInfo = { fileIdx, data, size };
      TransferActiveStream(handle, &transferFileInfo);
   }
}

void SimpleFsSDeviceDeleteFile(ThisHandle *handle, uint16_t fileIdx)
{
   SDeviceAssert(handle);

   if(!TryWriteStreamFile(handle, GetActiveWriteStream(handle), fileIdx, NULL, 0))
   {
      TransferWriteFileInfo transferFileInfo = { fileIdx, NULL, 0 };
      TransferActiveStream(handle, &transferFileInfo);
   }
}

size_t SimpleFsSDeviceGetMaxFileSize(ThisHandle *handle, uint16_t fileIdx)
{
   SDeviceAssert(handle);

   ReadStream stream = BuildActiveReadStream(handle);
   return ReadStreamMaxFileSize(handle, &stream, fileIdx);
}

size_t SimpleFsSDeviceReadFile(ThisHandle *handle, uint16_t fileIdx, void *buffer, size_t maxFileSize)
{
   SDeviceAssert(handle);

   SDeviceAssert(buffer);

   SDeviceAssert(maxFileSize > 0);

   ReadStream stream = BuildActiveReadStream(handle);
   return ReadStreamFile(handle, &stream, fileIdx, buffer, maxFileSize);
}
