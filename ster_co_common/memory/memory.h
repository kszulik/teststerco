/*
 * memory.h
 *
 *  Created on: Nov 8, 2024
 *      Author: kszulik
 */

#include <stddef.h>
#include <stdint.h>

#ifndef APPLICATION_USER_MEMORY_MEMORY_H_
#define APPLICATION_USER_MEMORY_MEMORY_H_

#define MAGIC_NUMBER 0xdeadbeef

#ifndef FW_VERSION_MAJOR
#define FW_VERSION_MAJOR 1
#endif

#ifndef FW_VERSION_MINOR
#define FW_VERSION_MINOR 0
#endif

#ifndef FW_VERSION_PATCH
#define FW_VERSION_PATCH 0
#endif

#define APPLICATION_ADDRESS 0x08010000
#define BOOTLOADER_SIZE 0x8000  // 32 KB

#define SRAM_BASE 0x20000000
#define SRAM_SIZE (320 * 1024)  // 320KB SRAM
#define SRAM_END (SRAM_BASE + SRAM_SIZE)


  /* Deklaracje extern symboli z linkera */
  extern void *__version_boot_start;
  extern void *__version_boot_end;
  extern void *__version_app_start;
  extern void *__version_app_end;
  extern void *isr_vector;

/* Struktura numeru wersji (3 bajty) */
typedef struct __attribute__((packed)) {
  uint32_t border_start;
  uint8_t major;
  uint8_t minor;
  uint8_t patch;
  uint8_t reserved;  // Dodatkowy bajt dla wyrównania
  uint32_t border_stop;
  uint32_t border_stop2;
} Version_t;

int versionInit(void);

#endif /* APPLICATION_USER_MEMORY_MEMORY_H_ */
