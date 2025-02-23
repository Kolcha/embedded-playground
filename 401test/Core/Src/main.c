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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <stdio.h>

#include "display.h"
#include "gpio_button.h"

#include "app_state_machine.h"
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

GPIO_Button ubtn = GPIO_BUTTON(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
GPIO_Button btn1 = GPIO_BUTTON(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
// pull-up resistor seems to be dead on pin B2, so use pull-down
GPIO_Button btn2 = GPIO_BUTTON(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);

static volatile bool time_to_render = true;

enum i2c_transfer_state_t {
  I2C_STATE_TRANSFER_CTRL_BYTES = 0x01,
  I2C_STATE_TRANSFER_DATA_BYTES = 0x02,
};

static volatile uint8_t i2c_state = 0u;
static volatile uint32_t fts = 0;
static volatile uint32_t ftt = 0;
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

static void button_m_handle_short_press()
{
  printf("%s\n", __FUNCTION__);
  app_sm.curr_state->api->button_m(&app_state_data);
}

static void button_1_handle_short_press()
{
  printf("%s\n", __FUNCTION__);
  app_sm.curr_state->api->button_1(&app_state_data);
}

static void button_2_handle_short_press()
{
  printf("%s\n", __FUNCTION__);
  app_sm.curr_state->api->button_2(&app_state_data);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  ubtn.click_handler = &button_m_handle_short_press;

  btn1.mode = GPIO_BUTTON_MODE_REPEAT;
  btn2.mode = GPIO_BUTTON_MODE_REPEAT;

  btn1.click_handler = &button_1_handle_short_press;
  btn2.click_handler = &button_2_handle_short_press;
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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(10);  // some delay is required to get display work from cold start
  ssd1306_init();
  HAL_TIM_Base_Start_IT(&htim10);
  app_state_machine_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint32_t lt = HAL_GetTick();
  while (1)
  {
    GPIO_ButtonCheckState(&ubtn);
    GPIO_ButtonCheckState(&btn1);
    GPIO_ButtonCheckState(&btn2);

    if (time_to_render && i2c_state == 0) {
      uint32_t rst = HAL_GetTick();
      uint32_t lft = rst - lt;
      app_sm.curr_state->api->render(&app_state_data, &ssd1306_128x32);
      uint32_t ret = HAL_GetTick();
      uint32_t frt = ret - rst;
      time_to_render = false;
      printf("since last frame: %4lu ms  rendering: %2lu ms  transfer: %lu ms\n", lft, frt, ftt);
      lt = ret;

      uint8_t ctr_byte = 0x40;
      i2c_state |= I2C_STATE_TRANSFER_CTRL_BYTES;
      fts = HAL_GetTick();
      HAL_I2C_Master_Seq_Transmit_DMA(&hi2c1, 0x3C << 1, &ctr_byte, 1, I2C_FIRST_AND_NEXT_FRAME);
    }
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
  time_to_render = true;
//  static const int ticks_on = 4;
//  static const int ticks_off = 150;
//  static int ticks_to_wait = ticks_off;
//  static int ticks_counter = 0;
//  if (++ticks_counter == ticks_to_wait) {
//    ticks_counter = 0;
//    ticks_to_wait = ticks_to_wait == ticks_off ? ticks_on : ticks_off;
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
//  }
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  if (i2c_state & I2C_STATE_TRANSFER_CTRL_BYTES) {
    i2c_state &= ~I2C_STATE_TRANSFER_CTRL_BYTES;
    i2c_state |= I2C_STATE_TRANSFER_DATA_BYTES;
    HAL_I2C_Master_Seq_Transmit_DMA(hi2c, 0x3C << 1, framebuffer, 512, I2C_LAST_FRAME);
    return;
  }

  if (i2c_state & I2C_STATE_TRANSFER_DATA_BYTES) {
    i2c_state &= ~I2C_STATE_TRANSFER_DATA_BYTES;
    ftt = HAL_GetTick() - fts;
    return;
  }
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
