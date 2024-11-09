/*
 * memory.c
 *
 *  Created on: Nov 8, 2024
 *      Author: kszulik
 */
#include "memory.h"
#include "stm32f7xx.h"




#ifdef BOOT
const __attribute__((used)) __attribute__((section(".boot_version_section"))) volatile Version_t bootVersion = {
    .border_start = MAGIC_NUMBER,
    .major = FW_VERSION_MAJOR,
    .minor = FW_VERSION_MINOR,
    .patch = FW_VERSION_PATCH,
    .reserved = 0x00,
    .border_stop = MAGIC_NUMBER,
    .border_stop2 = MAGIC_NUMBER};
#else
volatile const __attribute__((used)) __attribute__((section(".app_version_section")))  Version_t appVersion = {
    .border_start = MAGIC_NUMBER,
    .major = FW_VERSION_MAJOR,
    .minor = FW_VERSION_MINOR,
    .patch = FW_VERSION_PATCH,
    .reserved = 0x00,
    .border_stop = MAGIC_NUMBER,
    .border_stop2 = MAGIC_NUMBER};
#endif

volatile const Version_t *pAppVersion = (const Version_t *)&__version_app_start;
volatile const Version_t *pBootVersion = (const Version_t *)&__version_boot_start;

int versionInit(void) {

  if (pAppVersion == NULL || pBootVersion == NULL) {
    return 0;  // Błąd: Wskaznik jest NULL
  }

  /* Sprawdzenie, czy numer wersji jest prawidłowy */
   if (pBootVersion->border_start == 0xdeadbeef &&
       pBootVersion->major != 0xFF &&
       pBootVersion->minor != 0xFF &&
       pBootVersion->patch != 0xFF &&
       pBootVersion->border_stop == 0xdeadbeef &&
       pBootVersion->border_stop2 == 0xdeadbeef)
     {
       assert_param(1);
     }else
       {
	 assert_param(0);
       }

  /* Sprawdzenie, czy numer wersji jest prawidłowy */
   if (pAppVersion->border_start == 0xdeadbeef &&
       pAppVersion->major != 0xFF &&
       pAppVersion->minor != 0xFF &&
       pAppVersion->patch != 0xFF &&
       pAppVersion->border_stop == 0xdeadbeef &&
       pAppVersion->border_stop2 == 0xdeadbeef)
     {

     return 1; /* Numer wersji jest obecny i prawidłowy */
   } else {

#ifndef BOOT
       assert_param(0);
#endif
     return 0; /* Numer wersji nie jest obecny lub jest nieprawidłowy */
   }
}

