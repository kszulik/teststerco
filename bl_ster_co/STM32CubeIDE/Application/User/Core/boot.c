/*
 * boot.c
 *
 *  Created on: Nov 8, 2024
 *      Author: kszulik
 */

#include "boot.h"
#include "main.h"
#include "memory.h"

extern CRC_HandleTypeDef hcrc;

extern DMA2D_HandleTypeDef hdma2d;

extern I2C_HandleTypeDef hi2c3;

extern LTDC_HandleTypeDef hltdc;

extern QSPI_HandleTypeDef hqspi;

extern SDRAM_HandleTypeDef hsdram1;






void Jump_To_Application(void);

uint8_t Check_Application_Present(void);



uint8_t Check_Application_Present(void) {
  // Sprawdź, czy wskaźnik stosu w tablicy wektorów jest w zakresie SRAM
  uint32_t sp = *(volatile uint32_t *)APPLICATION_ADDRESS;
  if (sp < SRAM_BASE || sp > SRAM_END) {
    return 0;  // Nieprawidłowy wskaźnik stosu
  }

  // Sprawdź, czy wektor resetu wskazuje na prawidłowy adres flash
  uint32_t pc = *(volatile uint32_t *)(APPLICATION_ADDRESS + 4);
  if (pc < FLASH_BASE || pc > FLASH_END) {
    return 0;  // Nieprawidłowy wektor resetu
  }

  // Sprawdź, czy pierwsze kilka wektorów przerwań nie jest pustych (0xFFFFFFFF)
  for (int i = 0; i < 8; i++) {
    uint32_t vector = *(volatile uint32_t *)(APPLICATION_ADDRESS + i * 4);
    if (vector == 0xFFFFFFFF) {
      return 0;  // Znaleziono pusty wektor
    }
  }
  if(versionInit()!= 1)
    return 1;

  // Jeśli wszystkie sprawdzenia przejdą, zakładamy, że aplikacja jest obecna
  return 1;
}



void Jump_To_Application(void) {

  Peripherial_DeInit();
  // Disable SysTick
  SysTick->CTRL = 0;

  // Reset all peripherals
  HAL_DeInit();

  // Set the vector table to the application's vector table
  SCB->VTOR = APPLICATION_ADDRESS;

  // Set the stack pointer to the application's stack pointer
  __set_MSP(*(__IO uint32_t *)APPLICATION_ADDRESS);

  // Jump to the application
  ((void (*)(void))(*(__IO uint32_t *)(APPLICATION_ADDRESS + 4)))();
}



