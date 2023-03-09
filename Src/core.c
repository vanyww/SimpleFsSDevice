#include "Mid-layer/initialization.h"
#include "Mid-layer/integrity.h"

#include "SDeviceCore/heap.h"

SDEVICE_CREATE_HANDLE_DECLARATION(SimpleFS, init, parent, identifier, context)
{
   SDeviceAssert(init != NULL);

   const ThisInitData *_init = init;

   SDeviceAssert(_init->ReadUInt64 != NULL);
   SDeviceAssert(_init->WriteUInt64 != NULL);
   SDeviceAssert(_init->EraseSector != NULL);

#ifdef SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
   SDeviceAssert(_init->UpdateCrc8 != NULL);
   SDeviceAssert(_init->ComputeCrc8 != NULL);
   SDeviceAssert(_init->UpdateCrc16 != NULL);
   SDeviceAssert(_init->ComputeCrc16 != NULL);
#endif

   SDeviceAssert(HasSectorValidSize(&_init->Sector$0));
   SDeviceAssert(HasSectorValidSize(&_init->Sector$1));

   ThisHandle *handle = SDeviceMalloc(sizeof(ThisHandle));

   handle->Header = (SDeviceHandleHeader)
   {
      .Context = context,
      .ParentHandle = parent,
      .Identifier = identifier,
      .LatestStatus = SIMPLE_FS_SDEVICE_STATUS_OK
   };

   handle->Init = *_init;
   handle->Runtime = (ThisRuntimeData)
   {
      .Sector$0WriteStream = { .Sector = &handle->Init.Sector$0, .IsInBounds = false },
      .Sector$1WriteStream = { .Sector = &handle->Init.Sector$1, .IsInBounds = false },
      .InactiveWriteStream = NULL,
      .ActiveWriteStream = NULL
   };

   InitializeCrc8();
   InitializeCrc16();

   ProcessInitialMemoryState(handle);

   return handle;
}

SDEVICE_DISPOSE_HANDLE_DECLARATION(SimpleFS, handlePointer)
{
   SDeviceAssert(handlePointer != NULL);

   ThisHandle **_handlePointer = handlePointer;
   ThisHandle *handle = *_handlePointer;

   SDeviceAssert(handle != NULL);

   SDeviceFree(handle);
   *_handlePointer = NULL;
}

SDEVICE_GET_PROPERTY_DECLARATION(SimpleFs, TotalBadBlocksCount, handle, value)
{
   SDeviceAssert(value != NULL);
   SDeviceAssert(handle != NULL);

   size_t totalBadBlocksCount = ComputeTotalBadBlocksCount(handle);
   memcpy(value, &totalBadBlocksCount, sizeof(totalBadBlocksCount));

   return SDEVICE_PROPERTY_OPERATION_STATUS_OK;
}

void SimpleFsSDeviceFormatMemory(ThisHandle *handle)
{
   SDeviceAssert(handle != NULL);

   FormatMemory(handle);
}

void SimpleFsSDeviceForceHistoryDeletion(SDEVICE_HANDLE(SimpleFs) *handle)
{
   SDeviceAssert(handle != NULL);

   TransferActiveStream(handle, NULL);
}

void SimpleFsSDeviceWriteFile(ThisHandle *handle, uint16_t fileId, const void *data, size_t size)
{
   SDeviceAssert(size != 0 && size <= MAX_FILE_SIZE);
   SDeviceAssert(data != NULL);
   SDeviceAssert(handle != NULL);

   if(!TryWriteStreamFile(handle, GetActiveWriteStream(handle), fileId, data, size))
   {
      TransferWriteFileInfo transferFileInfo = { fileId, data, size };
      TransferActiveStream(handle, &transferFileInfo);
   }
}

void SimpleFsSDeviceDeleteFile(ThisHandle *handle, uint16_t fileId)
{
   SDeviceAssert(handle != NULL);

   if(!TryWriteStreamFile(handle, GetActiveWriteStream(handle), fileId, NULL, 0))
   {
      TransferWriteFileInfo transferFileInfo = { fileId, NULL, 0 };
      TransferActiveStream(handle, &transferFileInfo);
   }
}

size_t SimpleFsSDeviceGetMaxFileSize(ThisHandle *handle, uint16_t fileId)
{
   SDeviceAssert(handle != NULL);

   ReadStream stream = BuildActiveReadStream(handle);
   return ReadStreamMaxFileSize(handle, &stream, fileId);
}

size_t SimpleFsSDeviceReadFile(ThisHandle *handle, uint16_t fileId, void *buffer, size_t maxFileSize)
{
   SDeviceAssert(handle != NULL);
   SDeviceAssert(buffer != NULL);
   SDeviceAssert(maxFileSize > 0);

   ReadStream stream = BuildActiveReadStream(handle);
   return ReadStreamFile(handle, &stream, fileId, buffer, maxFileSize);
}
