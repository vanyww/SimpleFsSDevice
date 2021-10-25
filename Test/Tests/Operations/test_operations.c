#include "test_operations.h"
#include "../../Device/test_device.h"
#include "../../Device/Mock/Assertation/mock_assert.h"

#include <memory.h>

bool TestSmallWriteAndReadWithReinitialization(void)
{
   __SDEVICE_HANDLE(FlashFileSystem) writeHandle;
   CreateFlashFileSystemSDevice(&writeHandle, true);
   uint8_t dataToWrite[] = { 1, 2, 3, 4 };
   if(FlashFileSystemSDeviceWrite(&writeHandle,
                                  0,
                                  sizeof(dataToWrite),
                                  dataToWrite) != FLASH_FILE_SYSTEM_SDEVICE_STATE_OK)
      return false;

   __SDEVICE_HANDLE(FlashFileSystem) readHandle;
   CreateFlashFileSystemSDevice(&readHandle, false);

   size_t readSize;
   if(FlashFileSystemSDeviceGetVariableSize(&readHandle, 0, &readSize) != FLASH_FILE_SYSTEM_SDEVICE_STATE_OK)
      return false;

   if(readSize != sizeof(dataToWrite))
      return false;

   uint8_t readData[sizeof(dataToWrite)] = {0};
   if(FlashFileSystemSDeviceRead(&readHandle, 0, sizeof(readData), readData) != FLASH_FILE_SYSTEM_SDEVICE_STATE_OK)
      return false;

   return (memcmp(dataToWrite, readData, readSize) == 0) && (WasAssertFailed() == false);
}

bool TestLargeWriteAndReadWithReinitialization(void)
{
   __SDEVICE_HANDLE(FlashFileSystem) writeHandle;
   CreateFlashFileSystemSDevice(&writeHandle, true);
   uint8_t dataToWrite[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
   if(FlashFileSystemSDeviceWrite(&writeHandle,
                                  0,
                                  sizeof(dataToWrite),
                                  dataToWrite) != FLASH_FILE_SYSTEM_SDEVICE_STATE_OK)
      return false;

   __SDEVICE_HANDLE(FlashFileSystem) readHandle;
   CreateFlashFileSystemSDevice(&readHandle, false);

   size_t readSize;
   if(FlashFileSystemSDeviceGetVariableSize(&readHandle, 0, &readSize) != FLASH_FILE_SYSTEM_SDEVICE_STATE_OK)
      return false;

   if(readSize != sizeof(dataToWrite))
      return false;

   uint8_t readData[sizeof(dataToWrite)] = {0};
   if(FlashFileSystemSDeviceRead(&readHandle, 0, sizeof(readData), readData) != FLASH_FILE_SYSTEM_SDEVICE_STATE_OK)
      return false;

   return (memcmp(dataToWrite, readData, readSize) == 0) && (WasAssertFailed() == false);
}

bool TestMultipleWriteAndReadWithReinitialization(void)
{
   __SDEVICE_HANDLE(FlashFileSystem) writeHandle;
   CreateFlashFileSystemSDevice(&writeHandle, true);
   uint8_t *dataToWrite[2] =
   {
     (uint8_t[]){ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
     (uint8_t[]){ 11, 12, 13, 14, 15, 16, 17, 18, 19, 20 }
   };
   if(FlashFileSystemSDeviceWrite(&writeHandle,
                                  0,
                                  sizeof(dataToWrite[0]),
                                  dataToWrite[0]) != FLASH_FILE_SYSTEM_SDEVICE_STATE_OK)
      return false;

   if(FlashFileSystemSDeviceWrite(&writeHandle,
                                  0,
                                  sizeof(dataToWrite[1]),
                                  dataToWrite[1]) != FLASH_FILE_SYSTEM_SDEVICE_STATE_OK)
      return false;

   __SDEVICE_HANDLE(FlashFileSystem) readHandle;
   CreateFlashFileSystemSDevice(&readHandle, false);

   size_t readSize;
   if(FlashFileSystemSDeviceGetVariableSize(&readHandle, 0, &readSize) != FLASH_FILE_SYSTEM_SDEVICE_STATE_OK)
      return false;

   if(readSize != sizeof(dataToWrite[1]))
      return false;

   uint8_t readData[sizeof(dataToWrite[1])] = {0};
   if(FlashFileSystemSDeviceRead(&readHandle, 0, sizeof(readData), readData) != FLASH_FILE_SYSTEM_SDEVICE_STATE_OK)
      return false;

   return (memcmp(dataToWrite[1], readData, readSize) == 0) && (WasAssertFailed() == false);
}

bool TestMultipleVariablesWriteAndReadWithReinitialization(void)
{
   __SDEVICE_HANDLE(FlashFileSystem) writeHandle;
   CreateFlashFileSystemSDevice(&writeHandle, true);
   uint8_t fdataToWrite[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
   uint8_t sdataToWrite[] = { 11, 12, 13, 14, 15 };

   if(FlashFileSystemSDeviceWrite(&writeHandle,
                                  0,
                                  sizeof(fdataToWrite),
                                  fdataToWrite) != FLASH_FILE_SYSTEM_SDEVICE_STATE_OK)
      return false;

   if(FlashFileSystemSDeviceWrite(&writeHandle,
                                  1,
                                  sizeof(sdataToWrite),
                                  sdataToWrite) != FLASH_FILE_SYSTEM_SDEVICE_STATE_OK)
      return false;

   __SDEVICE_HANDLE(FlashFileSystem) readHandle;
   CreateFlashFileSystemSDevice(&readHandle, false);

   size_t readSize;
   if(FlashFileSystemSDeviceGetVariableSize(&readHandle, 0, &readSize) != FLASH_FILE_SYSTEM_SDEVICE_STATE_OK)
      return false;

   if(readSize != sizeof(fdataToWrite))
      return false;

   if(FlashFileSystemSDeviceGetVariableSize(&readHandle, 1, &readSize) != FLASH_FILE_SYSTEM_SDEVICE_STATE_OK)
      return false;

   if(readSize != sizeof(sdataToWrite))
      return false;

   uint8_t freadData[sizeof(fdataToWrite)] = {0};
   uint8_t sreadData[sizeof(sdataToWrite)] = {0};
   if(FlashFileSystemSDeviceRead(&readHandle, 0, sizeof(freadData), freadData) != FLASH_FILE_SYSTEM_SDEVICE_STATE_OK)
      return false;

   if(FlashFileSystemSDeviceRead(&readHandle, 1, sizeof(sreadData), sreadData) != FLASH_FILE_SYSTEM_SDEVICE_STATE_OK)
      return false;

   return (memcmp(fdataToWrite, freadData, sizeof(freadData)) == 0) &&
          (memcmp(sdataToWrite, sreadData, sizeof(sreadData)) == 0) &&
          (WasAssertFailed() == false);
}

bool TestTransferWithReinitialization(void)
{
   __SDEVICE_HANDLE(FlashFileSystem) writeHandle;
   CreateFlashFileSystemSDevice(&writeHandle, true);

   uint32_t lastDataToWrite = 25;

   for(uint32_t i = 0; i <= lastDataToWrite; i++)
   {
      if(FlashFileSystemSDeviceWrite(&writeHandle,
                                     0,
                                     sizeof(i),
                                     &i) != FLASH_FILE_SYSTEM_SDEVICE_STATE_OK)
         return false;
   }

   __SDEVICE_HANDLE(FlashFileSystem) readHandle;
   CreateFlashFileSystemSDevice(&readHandle, false);

   size_t readSize;
   if(FlashFileSystemSDeviceGetVariableSize(&readHandle, 0, &readSize) != FLASH_FILE_SYSTEM_SDEVICE_STATE_OK)
      return false;

   if(readSize != sizeof(lastDataToWrite))
      return false;

   uint8_t readData[sizeof(lastDataToWrite)] = {0};
   if(FlashFileSystemSDeviceRead(&readHandle, 0, readSize, readData) != FLASH_FILE_SYSTEM_SDEVICE_STATE_OK)
      return false;

   return (memcmp(&lastDataToWrite, readData, sizeof(lastDataToWrite)) == 0) &&
          (WasAssertFailed() == false);
}
