/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "cmsis_os.h"
#include "string.h"
#include "memory.h"
#include "boot.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stm32746g_discovery_qspi.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define REFRESH_COUNT 1835

#define SDRAM_TIMEOUT ((uint32_t)0xFFFF)
#define SDRAM_MODEREG_BURST_LENGTH_1 ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2 ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4 ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8 ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2 ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3 ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE ((uint16_t)0x0200)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

CRC_HandleTypeDef hcrc;

DMA2D_HandleTypeDef hdma2d;

I2C_HandleTypeDef hi2c3;

LTDC_HandleTypeDef hltdc;

QSPI_HandleTypeDef hqspi;

SDRAM_HandleTypeDef hsdram1;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
///* Definitions for TouchGFXTask */
// osThreadId_t TouchGFXTaskHandle;
// const osThreadAttr_t TouchGFXTask_attributes = {
//   .name = "TouchGFXTask",
//   .stack_size = 4096 * 4,
//   .priority = (osPriority_t) osPriorityNormal,
// };
///* Definitions for videoTask */
// osThreadId_t videoTaskHandle;
// const osThreadAttr_t videoTask_attributes = {
//   .name = "videoTask",
//   .stack_size = 1000 * 4,
//   .priority = (osPriority_t) osPriorityLow,
// };
/* USER CODE BEGIN PV */
static FMC_SDRAM_CommandTypeDef Command;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_CRC_Init(void);
static void MX_DMA2D_Init(void);
static void MX_FMC_Init(void);
static void MX_I2C3_Init(void);
static void MX_LTDC_Init(void);
static void MX_QUADSPI_Init(void);
void StartDefaultTask(void *argument);
extern void TouchGFX_Task(void *argument);
extern void videoTaskFunc(void *argument);

/* USER CODE BEGIN PFP */
BaseType_t IdleTaskHook(void *p);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

typedef void (*pFunction)(void);

#define FRAME_BUFFER_ADDRESS 0xC0000000
#define FRAME_BUFFER_WIDTH 480
#define FRAME_BUFFER_HEIGHT 272
#define FONT_WIDTH 10
#define FONT_HEIGHT 7
// Simple 5x7 font for ASCII characters 32-127
const uint8_t font[96][FONT_HEIGHT] = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // Space
    {0x04, 0x04, 0x04, 0x04, 0x00, 0x00, 0x04},  // !
    {0x0A, 0x0A, 0x0A, 0x00, 0x00, 0x00, 0x00},  // "
    {0x0A, 0x0A, 0x1F, 0x0A, 0x1F, 0x0A, 0x0A},  // #
    {0x04, 0x0F, 0x14, 0x0E, 0x05, 0x1E, 0x04},  // $
    {0x18, 0x19, 0x02, 0x04, 0x08, 0x13, 0x03},  // %
    {0x0C, 0x12, 0x14, 0x08, 0x15, 0x12, 0x0D},  // &
    {0x0C, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00},  // '
    {0x02, 0x04, 0x08, 0x08, 0x08, 0x04, 0x02},  // (
    {0x08, 0x04, 0x02, 0x02, 0x02, 0x04, 0x08},  // )
    {0x00, 0x04, 0x15, 0x0E, 0x15, 0x04, 0x00},  // *
    {0x00, 0x04, 0x04, 0x1F, 0x04, 0x04, 0x00},  // +
    {0x00, 0x00, 0x00, 0x00, 0x0C, 0x04, 0x08},  // ,
    {0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00},  // -
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C},  // .
    {0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x00},  // /
    {0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E},  // 0
    {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E},  // 1
    {0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F},  // 2
    {0x1F, 0x02, 0x04, 0x02, 0x01, 0x11, 0x0E},  // 3
    {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02},  // 4
    {0x1F, 0x10, 0x1E, 0x01, 0x01, 0x11, 0x0E},  // 5
    {0x06, 0x08, 0x10, 0x1E, 0x11, 0x11, 0x0E},  // 6
    {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08},  // 7
    {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E},  // 8
    {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x02, 0x0C},  // 9
    {0x00, 0x0C, 0x0C, 0x00, 0x0C, 0x0C, 0x00},  // :
    {0x00, 0x0C, 0x0C, 0x00, 0x0C, 0x04, 0x08},  // ;
    {0x02, 0x04, 0x08, 0x10, 0x08, 0x04, 0x02},  // <
    {0x00, 0x00, 0x1F, 0x00, 0x1F, 0x00, 0x00},  // =
    {0x08, 0x04, 0x02, 0x01, 0x02, 0x04, 0x08},  // >
    {0x0E, 0x11, 0x01, 0x02, 0x04, 0x00, 0x04},  // ?
    {0x0E, 0x11, 0x17, 0x15, 0x17, 0x10, 0x0F},  // @
    {0x04, 0x0A, 0x11, 0x11, 0x1F, 0x11, 0x11},  // A
    {0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E},  // B
    {0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E},  // C
    {0x1C, 0x12, 0x11, 0x11, 0x11, 0x12, 0x1C},  // D
    {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F},  // E
    {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10},  // F
    {0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0F},  // G
    {0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11},  // H
    {0x0E, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E},  // I
    {0x07, 0x02, 0x02, 0x02, 0x02, 0x12, 0x0C},  // J
    {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11},  // K
    {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F},  // L
    {0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11},  // M
    {0x11, 0x11, 0x19, 0x15, 0x13, 0x11, 0x11},  // N
    {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E},  // O
    {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10},  // P
    {0x0E, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0D},  // Q
    {0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11},  // R
    {0x0E, 0x11, 0x10, 0x0E, 0x01, 0x11, 0x0E},  // S
    {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04},  // T
    {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E},  // U
    {0x11, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x04},  // V
    {0x11, 0x11, 0x11, 0x15, 0x15, 0x1B, 0x11},  // W
    {0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11},  // X
    {0x11, 0x11, 0x0A, 0x04, 0x04, 0x04, 0x04},  // Y
    {0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1F},  // Z
    {0x0E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0E},  // [
    {0x00, 0x10, 0x08, 0x04, 0x02, 0x01, 0x00},  // '\'
    {0x0E, 0x02, 0x02, 0x02, 0x02, 0x02, 0x0E},  // ]
    {0x04, 0x0A, 0x11, 0x00, 0x00, 0x00, 0x00},  // ^
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F},  // _
    {0x08, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00},  // `
    {0x00, 0x00, 0x0E, 0x01, 0x0F, 0x11, 0x0F},  // a
    {0x10, 0x10, 0x16, 0x19, 0x11, 0x11, 0x1E},  // b
    {0x00, 0x00, 0x0E, 0x10, 0x10, 0x11, 0x0E},  // c
    {0x01, 0x01, 0x0D, 0x13, 0x11, 0x11, 0x0F},  // d
    {0x00, 0x00, 0x0E, 0x11, 0x1F, 0x10, 0x0E},  // e
    {0x06, 0x09, 0x08, 0x1C, 0x08, 0x08, 0x08},  // f
    {0x00, 0x0F, 0x11, 0x11, 0x0F, 0x01, 0x0E},  // g
    {0x10, 0x10, 0x16, 0x19, 0x11, 0x11, 0x11},  // h
    {0x04, 0x00, 0x0C, 0x04, 0x04, 0x04, 0x0E},  // i
    {0x02, 0x00, 0x06, 0x02, 0x02, 0x12, 0x0C},  // j
    {0x10, 0x10, 0x12, 0x14, 0x18, 0x14, 0x12},  // k
    {0x0C, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E},  // l
    {0x00, 0x00, 0x1A, 0x15, 0x15, 0x11, 0x11},  // m
    {0x00, 0x00, 0x16, 0x19, 0x11, 0x11, 0x11},  // n
    {0x00, 0x00, 0x0E, 0x11, 0x11, 0x11, 0x0E},  // o
    {0x00, 0x00, 0x1E, 0x11, 0x1E, 0x10, 0x10},  // p
    {0x00, 0x00, 0x0D, 0x13, 0x0F, 0x01, 0x01},  // q
    {0x00, 0x00, 0x16, 0x19, 0x10, 0x10, 0x10},  // r
    {0x00, 0x00, 0x0E, 0x10, 0x0E, 0x01, 0x1E},  // s
    {0x08, 0x08, 0x1C, 0x08, 0x08, 0x09, 0x06},  // t
    {0x00, 0x00, 0x11, 0x11, 0x11, 0x13, 0x0D},  // u
    {0x00, 0x00, 0x11, 0x11, 0x11, 0x0A, 0x04},  // v
    {0x00, 0x00, 0x11, 0x11, 0x15, 0x15, 0x0A},  // w
    {0x00, 0x00, 0x11, 0x0A, 0x04, 0x0A, 0x11},  // x
    {0x00, 0x00, 0x11, 0x11, 0x0F, 0x01, 0x0E},  // y
    {0x00, 0x00, 0x1F, 0x02, 0x04, 0x08, 0x1F},  // z
    {0x02, 0x04, 0x04, 0x08, 0x04, 0x04, 0x02},  // {
    {0x04, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04},  // |
    {0x08, 0x04, 0x04, 0x02, 0x04, 0x04, 0x08},  // }
    {0x00, 0x00, 0x0D, 0x12, 0x00, 0x00, 0x00},  // ~
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}   // DEL
};

void SetPixel(uint16_t *buffer, uint32_t x, uint32_t y, uint16_t color) {
  if (x < FRAME_BUFFER_WIDTH && y < FRAME_BUFFER_HEIGHT) {
    buffer[y * FRAME_BUFFER_WIDTH + x] = color;
  }
}

void DrawChar(uint16_t *buffer, char c, uint32_t x, uint32_t y,
              uint16_t color) {
  if (c < 32 || c > 127) return;  // Only support ASCII 32-127
  const uint8_t *bitmap = font[c - 32];
  for (uint32_t row = 0; row < FONT_HEIGHT; row++) {
    for (uint32_t col = 0; col < FONT_WIDTH; col++) {
      if (bitmap[row] & (1 << col)) {
        SetPixel(buffer, x + col, y + row, color);
      }
    }
  }
}

void DrawString(uint16_t *buffer, const char *str, uint32_t x, uint32_t y,
                uint16_t color) {
  while (*str) {
    DrawChar(buffer, *str++, x, y, color);
    x += FONT_WIDTH + 1;  // Move to the next character position
  }
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int boot_main(void) {
  /* USER CODE BEGIN 1 */
  versionInit();
  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CRC_Init();
  MX_DMA2D_Init();
  MX_FMC_Init();
  MX_I2C3_Init();
  MX_LTDC_Init();
  MX_QUADSPI_Init();

  /* USER CODE BEGIN 2 */

#define delay_time 1000

  // Get the frame buffer address
  uint16_t *frameBuffer = (uint16_t *)FRAME_BUFFER_ADDRESS;
  // Clear the frame buffer
  memset(frameBuffer, 0,
         FRAME_BUFFER_WIDTH * FRAME_BUFFER_HEIGHT * sizeof(uint16_t));
  // Draw a string on the screen
  DrawString(frameBuffer, "START BOOT", 100, 100, 0x001F);  // White color

  HAL_Delay(delay_time);

  memset(frameBuffer, 0,
         FRAME_BUFFER_WIDTH * FRAME_BUFFER_HEIGHT * sizeof(uint16_t));

  DrawString(frameBuffer, "CHECK FIRMWARE", 100, 100, 0x001F);  // White color

  HAL_Delay(delay_time);

  if (Check_Application_Present()) {
    memset(frameBuffer, 0,
           FRAME_BUFFER_WIDTH * FRAME_BUFFER_HEIGHT * sizeof(uint16_t));

    DrawString(frameBuffer, "RUN FIRMWARE", 100, 100, 0x001F);  // White color

    HAL_Delay(delay_time);

    Jump_To_Application();
  } else {
    HAL_Delay(delay_time);

    memset(frameBuffer, 0,
           FRAME_BUFFER_WIDTH * FRAME_BUFFER_HEIGHT * sizeof(uint16_t));

    DrawString(frameBuffer, "NO FIRMWARE DETECTED", 100, 100,
               0x001F);  // White color
    // No valid application, stay in bootloader
    // Here you can add code to wait for new firmware, e.g., via UART or USB
  }

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle =
      osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of TouchGFXTask */
  // TouchGFXTaskHandle = osThreadNew(TouchGFX_Task, NULL,
  // &TouchGFXTask_attributes);

  /* creation of videoTask */
  // videoTaskHandle = osThreadNew(videoTaskFunc, NULL, &videoTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
   */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK) {
    Error_Handler();
  }
}

/**
 * @brief CRC Initialization Function
 * @param None
 * @retval None
 */
static void MX_CRC_Init(void) {
  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
  if (HAL_CRC_Init(&hcrc) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */
}

/**
 * @brief DMA2D Initialization Function
 * @param None
 * @retval None
 */
static void MX_DMA2D_Init(void) {
  /* USER CODE BEGIN DMA2D_Init 0 */

  /* USER CODE END DMA2D_Init 0 */

  /* USER CODE BEGIN DMA2D_Init 1 */

  /* USER CODE END DMA2D_Init 1 */
  hdma2d.Instance = DMA2D;
  hdma2d.Init.Mode = DMA2D_M2M;
  hdma2d.Init.ColorMode = DMA2D_OUTPUT_ARGB8888;
  hdma2d.Init.OutputOffset = 0;
  hdma2d.LayerCfg[1].InputOffset = 0;
  hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;
  hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  hdma2d.LayerCfg[1].InputAlpha = 0;
  if (HAL_DMA2D_Init(&hdma2d) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_DMA2D_ConfigLayer(&hdma2d, 1) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN DMA2D_Init 2 */

  /* USER CODE END DMA2D_Init 2 */
}

/**
 * @brief I2C3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C3_Init(void) {
  /* USER CODE BEGIN I2C3_Init 0 */
  HAL_Delay(100);  // Delay to fix initialization issue on some boards
  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.Timing = 0x20404768;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK) {
    Error_Handler();
  }

  /** Configure Analogue filter
   */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK) {
    Error_Handler();
  }

  /** Configure Digital filter
   */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */
}

/**
 * @brief LTDC Initialization Function
 * @param None
 * @retval None
 */
static void MX_LTDC_Init(void) {
  /* USER CODE BEGIN LTDC_Init 0 */

  /* USER CODE END LTDC_Init 0 */

  LTDC_LayerCfgTypeDef pLayerCfg = {0};

  /* USER CODE BEGIN LTDC_Init 1 */

  /* USER CODE END LTDC_Init 1 */
  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Init.HorizontalSync = 40;
  hltdc.Init.VerticalSync = 9;
  hltdc.Init.AccumulatedHBP = 53;
  hltdc.Init.AccumulatedVBP = 11;
  hltdc.Init.AccumulatedActiveW = 533;
  hltdc.Init.AccumulatedActiveH = 283;
  hltdc.Init.TotalWidth = 565;
  hltdc.Init.TotalHeigh = 285;
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 0;
  hltdc.Init.Backcolor.Red = 0;
  if (HAL_LTDC_Init(&hltdc) != HAL_OK) {
    Error_Handler();
  }
  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = 480;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = 272;
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  pLayerCfg.Alpha = 255;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg.FBStartAdress = 0xC0000000;
  pLayerCfg.ImageWidth = 480;
  pLayerCfg.ImageHeight = 272;
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN LTDC_Init 2 */

  /* USER CODE END LTDC_Init 2 */
}

/**
 * @brief QUADSPI Initialization Function
 * @param None
 * @retval None
 */
static void MX_QUADSPI_Init(void) {
  /* USER CODE BEGIN QUADSPI_Init 0 */

  /* USER CODE END QUADSPI_Init 0 */

  /* USER CODE BEGIN QUADSPI_Init 1 */

  /* USER CODE END QUADSPI_Init 1 */
  /* QUADSPI parameter configuration*/
  hqspi.Instance = QUADSPI;
  hqspi.Init.ClockPrescaler = 1;
  hqspi.Init.FifoThreshold = 4;
  hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
  hqspi.Init.FlashSize = 24;
  hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_6_CYCLE;
  hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
  hqspi.Init.FlashID = QSPI_FLASH_ID_1;
  hqspi.Init.DualFlash = QSPI_DUALFLASH_DISABLE;
  if (HAL_QSPI_Init(&hqspi) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN QUADSPI_Init 2 */
  BSP_QSPI_Init();

  BSP_QSPI_MemoryMappedMode();
  HAL_NVIC_DisableIRQ(QUADSPI_IRQn);
  /* USER CODE END QUADSPI_Init 2 */
}

/* FMC initialization function */
static void MX_FMC_Init(void) {
  /* USER CODE BEGIN FMC_Init 0 */

  /* USER CODE END FMC_Init 0 */

  FMC_SDRAM_TimingTypeDef SdramTiming = {0};

  /* USER CODE BEGIN FMC_Init 1 */

  /* USER CODE END FMC_Init 1 */

  /** Perform the SDRAM1 memory initialization sequence
   */
  hsdram1.Instance = FMC_SDRAM_DEVICE;
  /* hsdram1.Init */
  hsdram1.Init.SDBank = FMC_SDRAM_BANK1;
  hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_8;
  hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_12;
  hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;
  hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;
  hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;
  hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
  hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0;
  /* SdramTiming */
  SdramTiming.LoadToActiveDelay = 2;
  SdramTiming.ExitSelfRefreshDelay = 7;
  SdramTiming.SelfRefreshTime = 4;
  SdramTiming.RowCycleDelay = 7;
  SdramTiming.WriteRecoveryTime = 3;
  SdramTiming.RPDelay = 2;
  SdramTiming.RCDDelay = 2;

  if (HAL_SDRAM_Init(&hsdram1, &SdramTiming) != HAL_OK) {
    Error_Handler();
  }

  /* USER CODE BEGIN FMC_Init 2 */
  __IO uint32_t tmpmrd = 0;

  /* Step 1: Configure a clock configuration enable command */
  Command.CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
  Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
  Command.AutoRefreshNumber = 1;
  Command.ModeRegisterDefinition = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(&hsdram1, &Command, SDRAM_TIMEOUT);

  /* Step 2: Insert 100 us minimum delay */
  /* Inserted delay is equal to 1 ms due to systick time base unit (ms) */
  HAL_Delay(1);

  /* Step 3: Configure a PALL (precharge all) command */
  Command.CommandMode = FMC_SDRAM_CMD_PALL;
  Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
  Command.AutoRefreshNumber = 1;
  Command.ModeRegisterDefinition = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(&hsdram1, &Command, SDRAM_TIMEOUT);

  /* Step 4: Configure an Auto Refresh command */
  Command.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
  Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
  Command.AutoRefreshNumber = 8;
  Command.ModeRegisterDefinition = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(&hsdram1, &Command, SDRAM_TIMEOUT);

  /* Step 5: Program the external memory mode register */
  tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1 |
           SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL | SDRAM_MODEREG_CAS_LATENCY_3 |
           SDRAM_MODEREG_OPERATING_MODE_STANDARD |
           SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

  Command.CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
  Command.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
  Command.AutoRefreshNumber = 1;
  Command.ModeRegisterDefinition = tmpmrd;

  /* Send the command */
  HAL_SDRAM_SendCommand(&hsdram1, &Command, SDRAM_TIMEOUT);

  /* Step 6: Set the refresh rate counter */
  /* Set the device refresh rate */
  HAL_SDRAM_ProgramRefreshRate(&hsdram1, REFRESH_COUNT);

  // Deactivate speculative/cache access to first FMC Bank to save FMC bandwidth
  FMC_Bank1->BTCR[0] = 0x000030D2;
  /* USER CODE END FMC_Init 2 */
}

void Peripherial_DeInit(void)
{
  // Disable all interrupts
  __disable_irq();

  // Stop all RTOS tasks and scheduler
  vTaskSuspendAll();
  taskENTER_CRITICAL();

  // Deinitialize all peripherals
  HAL_CRC_DeInit(&hcrc);
  HAL_DMA2D_DeInit(&hdma2d);
  HAL_I2C_DeInit(&hi2c3);
  HAL_LTDC_DeInit(&hltdc);
  HAL_QSPI_DeInit(&hqspi);
  HAL_SDRAM_DeInit(&hsdram1);

  // Reset clocks to default state
  HAL_RCC_DeInit();

  // Disable MPU
  HAL_MPU_Disable();

  // Disable and clean caches
  SCB_DisableICache();
  SCB_DisableDCache();
  SCB_InvalidateICache();
  SCB_CleanInvalidateDCache();
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(VSYNC_FREQ_GPIO_Port, VSYNC_FREQ_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_BL_CTRL_GPIO_Port, LCD_BL_CTRL_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_DISP_GPIO_Port, LCD_DISP_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, FRAME_RATE_Pin | RENDER_TIME_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(MCU_ACTIVE_GPIO_Port, MCU_ACTIVE_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : VSYNC_FREQ_Pin */
  GPIO_InitStruct.Pin = VSYNC_FREQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(VSYNC_FREQ_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_BL_CTRL_Pin */
  GPIO_InitStruct.Pin = LCD_BL_CTRL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_BL_CTRL_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_DISP_Pin */
  GPIO_InitStruct.Pin = LCD_DISP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_DISP_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : FRAME_RATE_Pin RENDER_TIME_Pin */
  GPIO_InitStruct.Pin = FRAME_RATE_Pin | RENDER_TIME_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : MCU_ACTIVE_Pin */
  GPIO_InitStruct.Pin = MCU_ACTIVE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(MCU_ACTIVE_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */


/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument) {
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for (;;) {
    osDelay(100);
  }
  /* USER CODE END 5 */
}

BaseType_t IdleTaskHook(void *p) {
  // Tutaj umieść kod, który ma być wykonywany przez IdleTaskHook
  // Na przykład:
  if ((int)p)  // idle task sched out
  {
    // Kod do wykonania, gdy zadanie bezczynności jest zaplanowane
  } else  // idle task sched in
  {
    // Kod do wykonania, gdy zadanie bezczynności jest uruchamiane
  }
  return pdTRUE;
}

/* MPU Configuration */

void MPU_Config(void) {
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
   */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x90000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_256MB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
   */
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.Size = MPU_REGION_SIZE_16MB;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM6 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
     number, tex: printf("Wrong parameters value: file %s on line %d\r\n", file,
     line) */
#ifdef DEBUG
  __BKPT(0);
#endif /* DEBUG */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
