#pragma once

/**
 * @file public.h
 * @brief Публичный интерфейс модуля файловой системы SimpleFsSDevice.
 * @details Объявления и определения, необходимые для пользовательского взаимодействия с модулем.
 * @n Пример использования:
 * @include core.h
 * В качестве файловой памяти в примере используется два массива (массив массивов).
 */

/**
 * @defgroup public Публичный интерфейс
 * @brief @copybrief public.h
 * @details @copydetails public.h
 * @{
 */

#include "log.h"
#include "config.h"
#include "dependensies.h"

#include <stdbool.h>

#if (SDEVICE_CORE_VERSION_MAJOR != 8) || (SDEVICE_CORE_VERSION_MINOR < 0)
#error SDevice core version is incorrect.
#endif

#ifdef DOXYGEN

/**
 * @brief **[Опция]** Флаг использования внешних функций для расчета CRC8 и CRC16.
 * @details Определение флага добавляет соответствующие функции обратного вызова в параметры инициализации модуля:
 * - @ref _SimpleFsSDeviceInitData::UpdateCrc8
 * - @ref _SimpleFsSDeviceInitData::ComputeCrc8
 * - @ref _SimpleFsSDeviceInitData::UpdateCrc16
 * - @ref _SimpleFsSDeviceInitData::ComputeCrc16
 *
 * @note Может быть объявлен пользователем.
 */
#define SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC


/**
 * @brief **[Опция]** Идентифиактор дескриптора модуля расчета CRC8.
 * @details Значение этого макроса будет использовано в качестве идентификатора дескриптора модуля расчета CRC8.
 * Используется только в случае отсутствия объявления флага #SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC.
 * @note Значение по умолчанию - 0.
 * @note Может быть объявлен пользователем.
 */
#define SIMPLE_FS_SDEVICE_TABLE_CRC8_INTERNAL_SDEVICE_IDENTIFIER 0

/**
 * @brief **[Опция]** Идентифиактор дескриптора модуля расчета CRC16.
 * @details Значение этого макроса будет использовано в качестве идентификатора дескриптора модуля расчета CRC16.
 * Используется только в случае отсутствия объявления флага #SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC.
 * @note Значение по умолчанию - 0.
 * @note Может быть объявлен пользователем.
 */
#define SIMPLE_FS_SDEVICE_TABLE_CRC16_INTERNAL_SDEVICE_IDENTIFIER 0

#endif

/**
 * @brief Старшая компонента версии модуля файловой системы SimpleFsSDevice.
 */
#define SIMPLE_FS_SDEVICE_VERSION_MAJOR 2

/**
 * @brief Средняя компонента версии модуля файловой системы SimpleFsSDevice.
 */
#define SIMPLE_FS_SDEVICE_VERSION_MINOR 0

/**
 * @brief Младшая компонента версии модуля файловой системы SimpleFsSDevice.
 */
#define SIMPLE_FS_SDEVICE_VERSION_PATCH 2

/**
 * @brief Версия модуля файловой системы SimpleFsSDevice в виде составного литерала структуры SDeviceVersion.
 */
#define SIMPLE_FS_SDEVICE_CORE_VERSION ((SDeviceVersion)                                                               \
{                                                                                                                      \
   .Major = SIMPLE_FS_SDEVICE_VERSION_MAJOR,                                                                           \
   .Minor = SIMPLE_FS_SDEVICE_VERSION_MINOR,                                                                           \
   .Patch = SIMPLE_FS_SDEVICE_VERSION_PATCH                                                                            \
})

/**
 * @brief Сектор файловой памяти.
 * @details Тип данных сектора памяти, используемого для хранения файлов.
 */
typedef struct
{
   void *Context; /**< Пользовательский контекст. */
   size_t Size; /**< Размер (в байтах) сектора. */
} SimpleFsSDeviceSector;

/**
 * @defgroup error_processing Обработка ошибок
 * @brief Идентификаторы, использующиеся при обработке ошибок и логировании состояний дескрипторов модуля.
 * @{
 */

/**
 * @brief Состояния дескрипторов модуля файловой системы SimpleFsSDevice.
 * @details Значения используются для логирования состояний дескрипторов модуля.
 */
typedef enum
{
   SIMPLE_FS_SDEVICE_EXCEPTION_OUT_OF_MEMORY, /**< Недостаток файловой памяти. */
   SIMPLE_FS_SDEVICE_EXCEPTION_BAD_AREA_OVERFLOW /**< Обнаружена слишком большая область "плохих" блоков. */
} SimpleFsSDeviceException;

/**
 * @brief Исключения дескрипторов модуля файловой системы SimpleFsSDevice.
 * @details Значения используются для выбрасывания исключений дескрипторов модуля.
 */
typedef enum
{
   SIMPLE_FS_SDEVICE_STATUS_OK, /**< Нормальная работа. */
   SIMPLE_FS_SDEVICE_STATUS_CORRUPTED_BLOCK_DETECTED, /**< Обнаружен поврежденный блок файловой памяти. */
   SIMPLE_FS_SDEVICE_STATUS_BAD_AREA_DETECTED /**< Обнаружена область "плохих" блоков. */
} SimpleFsSDeviceStatus;

/** @} */

/**
 * @defgroup handles Дескрипторы
 * @brief Инструменты управления дескрипторами модуля.
 * @{
 */

/**
 * @brief Предварительное объявление типа данных дескриптора модуля файловой системы SimpleFsSDevice.
 */
SDEVICE_HANDLE_FORWARD_DECLARATION(SimpleFs);
SDEVICE_INIT_DATA_FORWARD_DECLARATION(SimpleFs);

/**
 * @brief Параметры инициализации дескриптора модуля файловой системы SimpleFsSDevice.
 * @details Создаются пользователем для создания дескриптора модуля.
 */
SDEVICE_INIT_DATA_DECLARATION(SimpleFs)
{
   /**
    * @brief Функция чтения значения типа `uint64_t` из файловой памяти.
    * @details
    * @note Используется относительный адрес. Для хранения "нуля" можно использовать контекст сектора.
    * @note Гарантируется выравнивание адреса по 8-байтовой границе относительно "нуля".
    * @param handle Дескриптор.
    * @param sector Сектор, из которого необходимо произвести чтение.
    * @param address Относительный адрес в секторе @p sector, по которому необходимо произвести чтение.
    * @param value Буфер для читаемого значения.
    */
   void (* ReadUInt64)(SDEVICE_HANDLE(SimpleFs) *handle,
                       const SimpleFsSDeviceSector *sector,
                       uintptr_t address,
                       uint64_t *value);

   /**
    * @brief Функция записи значения типа `uint64_t` в файловую память.
    * @details
    * @note Используется относительный адрес. Для хранения "нуля" можно использовать контекст сектора.
    * @note Гарантируется выравнивание адреса по 8-байтовой границе относительно "нуля".
    * @param handle Дескриптор.
    * @param sector Сектор, в который необходимо произвести запись.
    * @param address Относительный адрес в секторе @p sector, по которому необходимо произвести запись.
    * @param value Записываемое значение.
    */
   void (* WriteUInt64)(SDEVICE_HANDLE(SimpleFs) *handle,
                        const SimpleFsSDeviceSector *sector,
                        uintptr_t address,
                        uint64_t value);

   /**
    * @brief Функция стирания сектора файловой памяти.
    * @param handle Дескриптор.
    * @param sector Сектор, который необходимо стереть.
    */
   void (* EraseSector)(SDEVICE_HANDLE(SimpleFs) *handle, const SimpleFsSDeviceSector *sector);

#ifdef SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC
   /**
    * @brief Функция обновления значения CRC8.
    * @details
    * @note Используется только в случае объявления флага #SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC.
    * @param handle Дескриптор.
    * @param crc Предыдущее значения CRC8.
    * @param data Данные, с помощью которых необходимо обновить значение @p crc.
    * @param size Размер данных @p data.
    * @returns Обновленное с помощью данных @p data значение @p crc.
    */
   uint8_t (* UpdateCrc8)(SDEVICE_HANDLE(SimpleFs) *handle, uint8_t crc, const void *data, size_t size);

   /**
    * @brief Функция вычисления значения CRC8.
    * @details
    * @note Используется только в случае объявления флага #SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC.
    * @param handle Дескриптор.
    * @param data Данные, для которых необходимо вычислить CRC8.
    * @param size Размер данных @p data.
    * @returns Значение CRC8 для данных @p data.
    */
   uint8_t (* ComputeCrc8)(SDEVICE_HANDLE(SimpleFs) *handle, const void *data, size_t size);

   /**
    * @brief Функция обновления значения CRC16.
    * @details
    * @note Используется только в случае объявления флага #SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC.
    * @param handle Дескриптор.
    * @param crc Предыдущее значения CRC16.
    * @param data Данные, с помощью которых необходимо обновить значение @p crc.
    * @param size Размер данных @p data.
    * @returns Обновленное с помощью данных @p data значение @p crc.
    */
   uint16_t (* UpdateCrc16)(SDEVICE_HANDLE(SimpleFs) *handle, uint16_t crc, const void *data, size_t size);

   /**
    * @brief Функция вычисления значения CRC16.
    * @details
    * @note Используется только в случае объявления флага #SIMPLE_FS_SDEVICE_USE_EXTERNAL_CRC.
    * @param handle Дескриптор.
    * @param data Данные, для которых необходимо вычислить CRC16.
    * @param size Размер данных @p data.
    * @returns Значение CRC16 для данных @p data.
    */
   uint16_t (* ComputeCrc16)(SDEVICE_HANDLE(SimpleFs) *handle, const void *data, size_t size);
#endif

   /**
    * @brief Нулевой сектор файловой памяти.
    */
   SimpleFsSDeviceSector Sector$0;

   /**
    * @brief Первый сектор файловой памяти.
    */
   SimpleFsSDeviceSector Sector$1;

   /**
    * @brief Флаг типа файловой памяти.
    * @details Указывает "пустое" (после стирания) значение файловой памяти:
    * - `true` - `0b0` (`0x00` для байта)
    * - `false` - `0b1` (`0xFF` для байта)
    */
   bool IsMemoryErasingToZero;
};

/**
 * @brief Функция создания дескриптора модуля файловой системы SimpleFsSDevice.
 * @param init Указатель на параметры инициализации дескриптора.
 * @param parent Внешний ("родительский") дескриптор.
 * @param identifier Значение идентификатора дескриптора.
 * @param context Контекст дескриптора.
 * @returns Дескриптор модуля файловой системы SimpleFsSDevice.
 */
SDEVICE_CREATE_HANDLE_DECLARATION(SimpleFs, init, parent, identifier, context);

/**
 * @brief Функция удаления дескриптора модуля файловой системы SimpleFsSDevice.
 * @param handlePointer Указатель на значение (указатель) дескриптора, который требуется удалить.
 */
SDEVICE_DISPOSE_HANDLE_DECLARATION(SimpleFs, handlePointer);

/** @} */

/**
 * @brief Тип данных значения свойства @ref _SimpleFsSDeviceTotalBadBlocksCountGetProperty.
 */
SDEVICE_PROPERTY_TYPE_DECLARATION(SimpleFs, TotalBadBlocksCount, size_t);

/**
 * @brief Свойство для чтения общего числа "плохих" блоков файловой памяти.
 * @details
 * @note Вычисляемое значение, вызывающее стирание истории в качестве стороннего эффекта.
 * @param handle Дескриптор.
 * @param value Буфер для читаемого значения типа @ref _SimpleFsSDeviceTotalBadBlocksCountPropertyType.
 * @returns Результат (состояние) операции чтения.
 */
SDEVICE_GET_PROPERTY_DECLARATION(SimpleFs, TotalBadBlocksCount, handle, value);

/**
 * @brief Функция форматирования файловой памяти.
 * @details Форматирует файловую память, стирая все файлы.
 * @param handle Дескриптор.
 */
void SimpleFsSDeviceFormatMemory(SDEVICE_HANDLE(SimpleFs) *handle);

/**
 * @brief Функция форсированного стирания файловой истории.
 * @details Инициирует стирание истории, перенося файлы в другой сектор.
 * @note Может использоваться для сброса "плохого" блока, если он возник в результате прерваной операции записи.
 * @param handle Дескриптор.
 */
void SimpleFsSDeviceForceHistoryDeletion(SDEVICE_HANDLE(SimpleFs) *handle);

/**
 * @brief Функция стирания файла.
 * @details
 * @note Фактическое стирание значения файла (и всей его истории) произойдет только при переносе на другой сектор.
 * Это можно форсировать функцией @ref SimpleFsSDeviceForceHistoryDeletion.
 * @param handle Дескриптор.
 * @param fileIdx Идентификатор файла.
 */
void SimpleFsSDeviceDeleteFile(SDEVICE_HANDLE(SimpleFs) *handle, uint16_t fileIdx);

/**
 * @brief Функция чтения максимального размера значения файла.
 * @details Под "максимальным" размером понимается наибольший размер значения среди всех записанных версий файла.
 * @param handle Дескриптор.
 * @param fileIdx Идентификатор файла.
 * @returns Максимальный размер значения файла с идентификатором @p fileIdx.
 */
size_t SimpleFsSDeviceGetMaxFileSize(SDEVICE_HANDLE(SimpleFs) *handle, uint16_t fileIdx);

/**
 * @brief Функция чтения значения файла.
 * @details Читает значение первой найденой консистентной версии файла, имеющей размер меньший или равный @p size.
 * @param handle Дескриптор.
 * @param fileIdx Идентификатор файла.
 * @param data Буфер для чтения значения файла.
 * @param size Максимальный размер читаемого файла.
 * @returns Фактический размер прочитанного в @p data значения файла (меньше или равен @p size).
 */
size_t SimpleFsSDeviceReadFile(SDEVICE_HANDLE(SimpleFs) *handle, uint16_t fileIdx, void *data, size_t size);

/**
 * @brief Функция записи значения файла.
 * @param handle Дескриптор.
 * @param fileIdx Идентификатор файла.
 * @param data Значение файла.
 * @param size Размер значения файла @p data.
 */
void SimpleFsSDeviceWriteFile(SDEVICE_HANDLE(SimpleFs) *handle, uint16_t fileIdx, const void *data, size_t size);

/** @} */
