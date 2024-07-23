#include "Mid-layer/initialization.h"
#include "Mid-layer/integrity.h"

#include "SDeviceCore/heap.h"

SDEVICE_IDENTITY_BLOCK_DEFINITION(
      SimpleFs,
      ((const SDeviceUuid)
      {
         .High = SIMPLE_FS_SDEVICE_UUID_HIGH,
         .Low  = SIMPLE_FS_SDEVICE_UUID_LOW
      }),
      ((const SDeviceVersion)
      {
         .Major = SIMPLE_FS_SDEVICE_VERSION_MAJOR,
         .Minor = SIMPLE_FS_SDEVICE_VERSION_MINOR,
         .Patch = SIMPLE_FS_SDEVICE_VERSION_PATCH
      }));

SDEVICE_CREATE_HANDLE_DECLARATION(SimpleFs, init, owner, identifier, context)
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

   SDeviceAssert(HasSectorValidSize(&_init->Sector$0));
   SDeviceAssert(HasSectorValidSize(&_init->Sector$1));

   ThisHandle *instance = SDeviceAllocateHandle(sizeof(*instance->Init), sizeof(*instance->Runtime));

   instance->Header = (SDeviceHandleHeader)
   {
      .Context       = context,
      .OwnerHandle   = owner,
      .IdentityBlock = &SDEVICE_IDENTITY_BLOCK(SimpleFs),
      .LatestStatus  = SIMPLE_FS_SDEVICE_STATUS_OK,
      .Identifier    = identifier
   };

   *instance->Init = *_init;

   *instance->Runtime = (ThisRuntimeData)
   {
      .Sector$0WriteStream = { .Sector = &instance->Init->Sector$0, .IsInBounds = false },
      .Sector$1WriteStream = { .Sector = &instance->Init->Sector$1, .IsInBounds = false },
      .InactiveWriteStream = NULL,
      .ActiveWriteStream   = NULL
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

   SDeviceAssert(IS_VALID_THIS_HANDLE(handle));

   SDeviceFreeHandle(handle);

   *_handlePointer = NULL;
}

SDEVICE_GET_SIMPLE_PROPERTY_DECLARATION(SimpleFs, TotalBadBlocksCount, handle, value)
{
   SDeviceAssert(IS_VALID_THIS_HANDLE(handle));

   SDeviceAssert(value);

   size_t totalBadBlocksCount = ComputeTotalBadBlocksCount(handle);
   memcpy(value, &totalBadBlocksCount, sizeof(totalBadBlocksCount));

   return SDEVICE_PROPERTY_STATUS_OK;
}

void SimpleFsSDeviceFormatMemory(ThisHandle *handle)
{
   SDeviceAssert(IS_VALID_THIS_HANDLE(handle));

   FormatMemory(handle);
}

void SimpleFsSDeviceForceHistoryDeletion(ThisHandle *handle)
{
   SDeviceAssert(IS_VALID_THIS_HANDLE(handle));

   TransferActiveStream(handle, NULL);
}

void SimpleFsSDeviceWriteFile(ThisHandle *handle, uint16_t fileIdx, const void *data, size_t size)
{
   SDeviceAssert(IS_VALID_THIS_HANDLE(handle));

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
   SDeviceAssert(IS_VALID_THIS_HANDLE(handle));

   if(!TryWriteStreamFile(handle, GetActiveWriteStream(handle), fileIdx, NULL, 0))
   {
      TransferWriteFileInfo transferFileInfo = { fileIdx, NULL, 0 };
      TransferActiveStream(handle, &transferFileInfo);
   }
}

size_t SimpleFsSDeviceGetMaxFileSize(ThisHandle *handle, uint16_t fileIdx)
{
   SDeviceAssert(IS_VALID_THIS_HANDLE(handle));

   ReadStream stream = BuildActiveReadStream(handle);
   return ReadStreamMaxFileSize(handle, &stream, fileIdx);
}

size_t SimpleFsSDeviceReadFile(ThisHandle *handle, uint16_t fileIdx, void *buffer, size_t maxFileSize)
{
   SDeviceAssert(IS_VALID_THIS_HANDLE(handle));

   SDeviceAssert(buffer);

   SDeviceAssert(maxFileSize > 0);

   ReadStream stream = BuildActiveReadStream(handle);
   return ReadStreamFile(handle, &stream, fileIdx, buffer, maxFileSize);
}
