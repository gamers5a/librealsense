#pragma once

#ifndef DS4_CAMERA_SPI_H
#define DS4_CAMERA_SPI_H

#include "libuvc/libuvc.h"
#include "R200_CalibRectParameters.h"
#include "R200_XU.h"

// SPI
#define SPI_FLASH_PAGE_SIZE_IN_BYTES                        0x100
#define SPI_FLASH_SECTOR_SIZE_IN_BYTES                      0x1000
#define SPI_FLASH_SIZE_IN_SECTORS                           256
#define SPI_FLASH_TOTAL_SIZE_IN_BYTES                       (SPI_FLASH_SIZE_IN_SECTORS * SPI_FLASH_SECTOR_SIZE_IN_BYTES)
#define SPI_FLASH_PAGES_PER_SECTOR                          (SPI_FLASH_SECTOR_SIZE_IN_BYTES / SPI_FLASH_PAGE_SIZE_IN_BYTES)
#define SPI_FLASH_LENGTH_IN_PAGES(N_BYTES)                  ((N_BYTES + 0xFF) / SPI_FLASH_PAGE_SIZE_IN_BYTES)

#define SPI_FLASH_SECTOR_INDEX(BYTE_ADDRESS)                (BYTE_ADDRESS >> 12)
#define SPI_FLASH_ADDRESS_WITHIN_SECTOR(BYTE_ADDRESS)       (BYTE_ADDRESS & 0xFFF)
#define SPI_FLASH_SECTOR_ADDRESS(BYTE_ADDRESS)              (BYTE_ADDRESS & ~0xFFF)
#define SPI_FLASH_SECTOR_ADDRESS_FROM_INDEX(SECTOR_INDEX)   (SECTOR_INDEX << 12)

#define SPI_FLASH_SECTORS_RESERVED_FOR_FIRMWARE             160
#define SPI_FLASH_START_OF_SECTORS_NOT_FOR_FIRMWARE         (SPI_FLASH_SECTORS_RESERVED_FOR_FIRMWARE * SPI_FLASH_SECTOR_SIZE_IN_BYTES)

#define SPI_FLASH_SECTORS_RESERVED_FOR_ROUTINES             64
#define SPI_FLASH_FIRST_ROUTINE_SECTOR                      (SPI_FLASH_SIZE_IN_SECTORS - SPI_FLASH_SECTORS_RESERVED_FOR_ROUTINES)

// 1 Mb total
#define NV_STORAGE_IN_BYTES                                 (SPI_FLASH_SECTOR_SIZE_IN_BYTES * SPI_FLASH_SIZE_IN_SECTORS)
#define NV_NON_FIRMWARE_START                               (SPI_FLASH_SECTORS_RESERVED_FOR_FIRMWARE * SPI_FLASH_SECTOR_SIZE_IN_BYTES)

#define NV_ADMIN_DATA_N_ENTRIES                             9
#define NV_CALIBRATION_DATA_ADDRESS_INDEX                   0
#define NV_IFFLEY_ROUTINE_TABLE_ADDRESS_INDEX               1
#define NV_IFFLEY_CONSTANTS_TABLE_ADDRESS_INDEX             2
#define NV_ENUMERATION_INFORMATION_ADDRESS_INDEX            3
#define NV_DESCRIPTOR_INFORMATION_ADDRESS_INDEX             4
#define IFFLEY_VERSION_NUMBER_INDEX                         5
#define FIRMWARE_VERSION_NUMBER_INDEX                       6
#define NV_CALIBRATION_DATA_BACKUP_ADDRESS_INDEX            7
#define NV_ENCRYPTED_HASH_DATA_INDEX                        8

#define NV_NON_FIRMWARE_ROOT_ADDRESS                        NV_NON_FIRMWARE_START
#define NV_INITIAL_CALIBRATION_DATA_ADDRESS                 (NV_NON_FIRMWARE_ROOT_ADDRESS + (NV_CALIBRATION_DATA_ADDRESS_INDEX + 1) * SPI_FLASH_SECTOR_SIZE_IN_BYTES)
#define NV_INITIAL_IFFLEY_ROUTINE_TABLE_ADDRESS             (NV_NON_FIRMWARE_ROOT_ADDRESS + (NV_IFFLEY_ROUTINE_TABLE_ADDRESS_INDEX + 1) * SPI_FLASH_SECTOR_SIZE_IN_BYTES)
#define NV_INITIAL_IFFLEY_CONSTANTS_TABLE_ADDRESS           (NV_NON_FIRMWARE_ROOT_ADDRESS + (NV_IFFLEY_CONSTANTS_TABLE_ADDRESS_INDEX + 1) * SPI_FLASH_SECTOR_SIZE_IN_BYTES)
#define NV_INITIAL_ENUMERATION_INFORMATION_TABLE_ADDRESS    (NV_NON_FIRMWARE_ROOT_ADDRESS + (NV_ENUMERATION_INFORMATION_ADDRESS_INDEX + 1) * SPI_FLASH_SECTOR_SIZE_IN_BYTES)
#define NV_INITIAL_DESCRIPTOR_INFORMATION_TABLE_ADDRESS     (NV_NON_FIRMWARE_ROOT_ADDRESS + (NV_DESCRIPTOR_INFORMATION_ADDRESS_INDEX + 1) * SPI_FLASH_SECTOR_SIZE_IN_BYTES)
#define NV_INITIAL_CALIBRATION_DATA_BACKUP_ADDRESS          (NV_INITIAL_DESCRIPTOR_INFORMATION_TABLE_ADDRESS + SPI_FLASH_SECTOR_SIZE_IN_BYTES)
#define NV_INITIAL_ENCRYPTED_HASH_DATA_ADDRESS              (NV_INITIAL_CALIBRATION_DATA_BACKUP_ADDRESS + SPI_FLASH_SECTOR_SIZE_IN_BYTES)

#define UNUSED_ROUTINE(ENTRY)                               (ENTRY == UNINITIALIZED_ROUTINE_ENTRY || ENTRY == DELETED_ROUTINE_ENTRY)
typedef unsigned short RoutineDescription;

#define MAX_ROUTINES                                        256

#define SIZEOF_ROUTINE_DESCRIPTION_TABLE                    (MAX_ROUTINES * sizeof(RoutineDescription))
#define SIZEOF_ERASED_TABLE                                 (SPI_FLASH_SECTORS_RESERVED_FOR_ROUTINES * sizeof(unsigned short))
#define SIZEOF_PRESERVE_TABLE                               (SPI_FLASH_SECTORS_RESERVED_FOR_ROUTINES * sizeof(unsigned short))

#define SIZEOF_ROUTINE_DESCRIPTION_ERASED_AND_PRESERVE_TABLE (SIZEOF_ROUTINE_DESCRIPTION_TABLE + SIZEOF_ERASED_TABLE + SIZEOF_PRESERVE_TABLE)

#define ROUTINE_DESCRIPTION_OFFSET                          0
#define ERASED_TABLE_OFFSET                                 SIZEOF_ROUTINE_DESCRIPTION_TABLE
#define PRESERVE_TABLE_OFFSET                               (ERASED_TABLE_OFFSET + SIZEOF_ERASED_TABLE)

typedef struct
{
    RoutineDescription rd[MAX_ROUTINES]; // Partition Table
    unsigned short erasedTable[SPI_FLASH_SECTORS_RESERVED_FOR_ROUTINES];
    unsigned short preserveTable[SPI_FLASH_SECTORS_RESERVED_FOR_ROUTINES];
} RoutineStorageTables;

// Bus group
#define COMMAND_MODIFIER_DIRECT 0x00000010

#define CAM_INFO_BLOCK_LEN 2048

//@tofix error handling all up in this bitch
class SPI_Interface
{
    DSCalibRectParameters parameters;
    
    uint8_t cameraHeader[SPI_FLASH_SECTOR_SIZE_IN_BYTES];
    uint8_t calibrationData[CAM_INFO_BLOCK_LEN];
    
    void ReadCalibrationSector();
    
    uvc_device_handle_t * deviceHandle;
    
public:
    
    RoutineStorageTables rst;
    
    SPI_Interface(uvc_device_handle_t * deviceHandle);
    void Initialize();
    
    void PrintHeaderInfo();

    DSCalibRectParameters GetRectifiedParameters() { return parameters; }
    DSCalibIntrinsicsRectified GetZIntrinsics(int mode = 0);
    
    
    
};

#endif
