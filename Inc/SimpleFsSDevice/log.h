#pragma once

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
   SIMPLE_FS_SDEVICE_STATUS_OK, /**< Нормальная работа. */
   SIMPLE_FS_SDEVICE_STATUS_CORRUPTED_BLOCK_DETECTED, /**< Обнаружен поврежденный блок файловой памяти. */
   SIMPLE_FS_SDEVICE_STATUS_BAD_AREA_DETECTED /**< Обнаружена область "плохих" блоков. */
} SimpleFsSDeviceStatus;

/** @} */
