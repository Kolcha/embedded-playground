/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "i2s.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <string.h>

#include "display.h"
#include "lat1-08.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static uint8_t framebuffer[512];

static const display_t ssd1306_128x32 = {
  .w = 128,
  .h = 32,
  .buf_sz = sizeof(framebuffer),
  .buf = framebuffer,
};

static const bmp_font_t lat1_08 = {
  .w = FONT_CHAR_W_LAT1_08,
  .h = FONT_CHAR_H_LAT1_08,
  .n = FONT_LENGTH_LAT1_08,
  .csz = FONT_CHARSZ_LAT1_08,
  .data = font_data_lat1_08,
};

static const render_context_t rctx = {
  .disp = &ssd1306_128x32,
  .font = &lat1_08,
};

static volatile bool transfer_frame = false;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static void ssd1306_write_cmd_byte(uint8_t addr, uint8_t cmd)
{
  uint8_t data[] = {0x00, cmd};
  HAL_I2C_Master_Transmit(&hi2c1, addr << 1, data, sizeof(data), 1);
}

static void ssd1306_write_cmd_bytes(uint8_t addr,
                                    const uint8_t* bytes, unsigned int count)
{
  for (unsigned int i = 0; i < count; i++)
    ssd1306_write_cmd_byte(addr, bytes[i]);
}

static void ssd1306_init()
{
  // 128x32 display is assumed, must be adjusted for another one
  const uint8_t init_seq[] = {
    // the sequence is according to datasheet
    0xA8, 0x1F,                 // MUX Ratio, 31 (H - 1)
    0xD3, 0x00,			// display offset, 0
    0x40 | 0x00,		// display start line, 0
    0xA0 | 0x00,                // segment (column) re-map (A0/A1)
    0xC0 | 0x00,                // scan direction (C0/C8)
    0xDA, 0x02,                 // wiring configuration
    0x81, 0x7F,			// contrast
    0xA4,			// follow RAM
    0xA6,			// normal/inverse (A6/A7)
    0xD5, 0x80,			// oscillator frequency
    0x8D, 0x14,			// charge pump regulator
    0xAF,			// display on
    // addressing
    0x20, 0x01,                 // vertical scanning
    0x21, 0x00, 0x7F,           // column start/end, [0, 127] (W - 1)
    0x22, 0x00, 0x03,           // page start/end, [0, 3] (H/8 - 1)
  };
  ssd1306_write_cmd_bytes(0x3C, init_seq, sizeof(init_seq));
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2S2_Init();
  MX_I2S3_Init();
  MX_TIM3_Init();
  MX_I2C1_Init();
  MX_TIM10_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(10);  // some delay is required to get display work from cold start
  ssd1306_init();
  HAL_TIM_Base_Start_IT(&htim10);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
  PeriphClkInitStruct.PLLI2S.PLLI2SN = 192;
  PeriphClkInitStruct.PLLI2S.PLLI2SR = 5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
//  static const int ticks_on = 4;
//  static const int ticks_off = 150;
//  static int ticks_to_wait = ticks_off;
//  static int ticks_counter = 0;
//  if (++ticks_counter == ticks_to_wait) {
//    ticks_counter = 0;
//    ticks_to_wait = ticks_to_wait == ticks_off ? ticks_on : ticks_off;
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
//  }

  static bool bw = true;
  memset(framebuffer, bw ? 0x00 : 0xFF, sizeof(framebuffer));
  framebuffer[0] = bw ? 0x01 : 0xFE;
  framebuffer[3] = bw ? 0x80 : 0x7F;
  framebuffer[508] = framebuffer[0];
  framebuffer[511] = framebuffer[3];
  bw = !bw;

  draw_text(&rctx, 48, 4, "Test\nText\nLine");

  if (transfer_frame) return;		// should not happen, but still

  uint8_t ctr_byte = 0x40;
  transfer_frame = true;
  HAL_I2C_Master_Seq_Transmit_DMA(&hi2c1, 0x3C << 1, &ctr_byte, 1, I2C_FIRST_AND_NEXT_FRAME);
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  if (!transfer_frame) return;		// callback after frame transfer
  HAL_I2C_Master_Seq_Transmit_DMA(&hi2c1, 0x3C << 1, framebuffer, 512, I2C_LAST_FRAME);
  transfer_frame = false;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
