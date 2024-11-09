/*
 * boot.h
 *
 *  Created on: Nov 8, 2024
 *      Author: kszulik
 */

#ifndef APPLICATION_USER_CORE_BOOT_H_
#define APPLICATION_USER_CORE_BOOT_H_

#include <stdint.h>
#include "stm32f7xx_hal.h"

void Jump_To_Application(void);
uint8_t Check_Application_Present(void);

#endif /* APPLICATION_USER_CORE_BOOT_H_ */
