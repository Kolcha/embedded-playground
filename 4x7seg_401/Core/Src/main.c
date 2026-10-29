/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// to get printf() work
int __io_putchar(int ch)
{
  if (ch == '\n') {
    static uint8_t cr = '\r';
    HAL_UART_Transmit(&huart1, &cr, 1, 1);
  }
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 1);
  return ch;
}

struct segment_t {
  GPIO_TypeDef* port;
  uint16_t pin;
};

static struct segment_t const g_segments[] = {
  { .port = SEG_A_GPIO_Port, .pin = SEG_A_Pin },
  { .port = SEG_B_GPIO_Port, .pin = SEG_B_Pin },
  { .port = SEG_C_GPIO_Port, .pin = SEG_C_Pin },
  { .port = SEG_D_GPIO_Port, .pin = SEG_D_Pin },
  { .port = SEG_E_GPIO_Port, .pin = SEG_E_Pin },
  { .port = SEG_F_GPIO_Port, .pin = SEG_F_Pin },
  { .port = SEG_G_GPIO_Port, .pin = SEG_G_Pin },
  { .port = SEG_H_GPIO_Port, .pin = SEG_H_Pin },
};

static void display_segments(uint8_t s)
{
  for (uint8_t i = 0; i < 8; i++) {
    GPIO_PinState state = (s & (1 << i)) > 0 ? GPIO_PIN_SET : GPIO_PIN_RESET;
    HAL_GPIO_WritePin(g_segments[i].port, g_segments[i].pin, state);
  }
}

static uint8_t const g_numbers[] = {
  (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (0 << 6),
  (0 << 0) | (1 << 1) | (1 << 2) | (0 << 3) | (0 << 4) | (0 << 5) | (0 << 6),
  (1 << 0) | (1 << 1) | (0 << 2) | (1 << 3) | (1 << 4) | (0 << 5) | (1 << 6),
  (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (0 << 4) | (0 << 5) | (1 << 6),
  (0 << 0) | (1 << 1) | (1 << 2) | (0 << 3) | (0 << 4) | (1 << 5) | (1 << 6),
  (1 << 0) | (0 << 1) | (1 << 2) | (1 << 3) | (0 << 4) | (1 << 5) | (1 << 6),
  (1 << 0) | (0 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6),
  (1 << 0) | (1 << 1) | (1 << 2) | (0 << 3) | (0 << 4) | (0 << 5) | (0 << 6),
  (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6),
  (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (0 << 4) | (1 << 5) | (1 << 6),

  (1 << 0) | (1 << 1) | (1 << 2) | (0 << 3) | (1 << 4) | (1 << 5) | (1 << 6),
  (0 << 0) | (0 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6),
  (1 << 0) | (0 << 1) | (0 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (0 << 6),
  (0 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (0 << 5) | (1 << 6),
  (1 << 0) | (0 << 1) | (0 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6),
  (1 << 0) | (0 << 1) | (0 << 2) | (0 << 3) | (1 << 4) | (1 << 5) | (1 << 6),
};

static void display_number(int v, bool add_dot)
{
  if (v < 0 || v > 16) return;
  display_segments(g_numbers[v] | (add_dot ? 0x80 : 0x00));
}

static void set_active_digit(int d)
{
  if (d < 0 || d > 3) return;
  htim4.Instance->CCER &= ~TIM_CCER_CCxE_MASK;
  htim4.Instance->CCER |= (1 << d*4);
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

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_TIM10_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
  HAL_TIM_Base_Start_IT(&htim10);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);

  htim4.Instance->CCR1 = htim4.Instance->ARR / 4;
  htim4.Instance->CCR2 = htim4.Instance->ARR / 4;
  htim4.Instance->CCR3 = htim4.Instance->ARR / 4;
  htim4.Instance->CCR4 = htim4.Instance->ARR / 4;
/*
  HAL_GPIO_WritePin(SEG_A_GPIO_Port, SEG_A_Pin, SET);
  HAL_GPIO_WritePin(SEG_B_GPIO_Port, SEG_B_Pin, SET);
  HAL_GPIO_WritePin(SEG_C_GPIO_Port, SEG_C_Pin, SET);
  HAL_GPIO_WritePin(SEG_D_GPIO_Port, SEG_D_Pin, SET);
  HAL_GPIO_WritePin(SEG_E_GPIO_Port, SEG_E_Pin, SET);
  HAL_GPIO_WritePin(SEG_F_GPIO_Port, SEG_F_Pin, SET);
  HAL_GPIO_WritePin(SEG_G_GPIO_Port, SEG_G_Pin, SET);
  HAL_GPIO_WritePin(SEG_H_GPIO_Port, SEG_H_Pin, SET);
*/
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  int d = 0;
//  uint8_t const data[] = {0x39, 0x67, 0x7C, 0x0F};
  int values[] = {1, 2, 3, 4};
  htim3.Instance->CNT = htim3.Instance->ARR / 2 + 1;
  while (1)
  {
//    int cnt_dwn = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim3);
//    printf("val: %lu, dwn: %d\n", htim3.Instance->CNT, cnt_dwn);
    if (d == 0) {
      long cnt = htim3.Instance->CNT;
      values[0] = cnt / 1e3; cnt -= values[0] * 1e3;
      values[1] = cnt / 1e2; cnt -= values[1] * 1e2;
      values[2] = cnt / 1e1; cnt -= values[2] * 1e1;
      values[3] = cnt;
    }
    set_active_digit(d);
//    display_segments(data[d]);
    display_number(values[d], false);
    HAL_Delay(2);
    if (++d == 4) d = 0;
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
/*
    for (int i = 0; i < 16; i++) {
      display_number(i, false);
      HAL_Delay(1500);
    }
*/
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

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == KEY_Pin) {
    htim3.Instance->CNT = htim3.Instance->ARR / 2 + 1;
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
#ifdef USE_FULL_ASSERT
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
