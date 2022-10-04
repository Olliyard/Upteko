/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "EPD_2in66.h"
#include "GUI_Paint.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
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
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef hlpuart1;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* USER CODE BEGIN PV */
static const uint8_t BQ76940_ADDR = 0x08<<1;//BMS temp address
static const uint8_t SYS_STAT_REG = 0x00;	//SYS_STAT register
static const uint8_t SYS_CTRL1_REG = 0x04;	//SYS_CTRL1 register
static const uint8_t SYS_CTRL2_REG = 0x05;	//SYS_CTRL2 register
static const uint8_t VC1_HI_REG = 0x0C;		//Needed to start the for() sequence later.
static const uint8_t TS1_HI_REG = 0x2C;		//TS1 register
static const uint8_t CC_HI_REG = 0x32;		//CC register
static const uint8_t ADCGAIN_REG = 0x50; 	//ADC gain register
static const uint8_t ADCOFFSET_REG = 0x51; 	//ADC offset register
HAL_StatusTypeDef ret;						//Error handle value
static const uint32_t WakeUpTimer = 0x4E20; //RTC wakeup timer value (20000 = 4E20)
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_LPUART1_UART_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */
void BQ76940_set_register(uint8_t reg, uint8_t reg_val);
void BQ76940_get_register(uint8_t reg, uint8_t* buf);
uint16_t BQ76940_get_HILO_register(uint8_t reg, uint8_t* buf);
void BQ76940_SHIPMODE();
void STM32L432KB_LP_STOP2();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	uint8_t buf_ADC_gain[8], buf_ADC_offset[8], buf_ALERT_stat[8];
	uint8_t buf_cell_volt[16], buf_temp[16], buf_current[16], adc_gain, adc_offset;
	uint16_t cell_volt, temp, current;
	//float pack_current;
	float pack_voltage;
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
  MX_LPUART1_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  BQ76940_get_register(ADCGAIN_REG, buf_ADC_gain);
  BQ76940_get_register(ADCOFFSET_REG, buf_ADC_offset);
  BQ76940_set_register(SYS_CTRL1_REG, 0x08u);	//Enable temperature measurements (TEMP_SEL = 1)
  BQ76940_set_register(SYS_CTRL2_REG, 0x40u);	//Enable CC ALWAYS ON mode (CC_EN = 1)
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);	//Set SPI CS pin high PA6
  EPD_2IN66_Init();	//Initialize the E-paper display
  EPD_2IN66_Clear();
  adc_gain = (buf_ADC_gain[0]*(10*exp(-6)));
  adc_offset = (buf_ADC_offset[0]*(10*exp(-3)));

  //Initial error checking for the SYS_STAT register.
  BQ76940_get_register(SYS_STAT_REG, buf_ALERT_stat);
  for(int i = 0; i<sizeof(buf_ALERT_stat); i++){
	  if(buf_ALERT_stat[i] != 0x00u){
		  printf("%d ", buf_ALERT_stat[i]);
		  BQ76940_set_register(SYS_STAT_REG, 0xFFu);
	  }
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  HAL_Delay(1000);
	  //Get each cell reading from their respective registers and store the voltage according to ADC gain and offset.
	  for(size_t i = 0; i<sizeof(buf_cell_volt); i++)
	  {
		  cell_volt = BQ76940_get_HILO_register(VC1_HI_REG + (i*2), buf_cell_volt);
		  cell_volt = ((adc_gain * cell_volt)+adc_offset);
		  printf("Cell %d voltage: %d\n", i, cell_volt);

		  //calculate pack total voltage from individual cell voltages
		  pack_voltage += cell_volt;
	  }
	  printf("Total cell voltage: %d\n", pack_voltage);

	  //get the equivalent temperature resistance value
	  temp = BQ76940_get_HILO_register(TS1_HI_REG, buf_temp);
	  printf("Temperature: %d\n", temp);

	  //get the pack current value
	  current = BQ76940_get_HILO_register(CC_HI_REG, buf_current);
	  printf("Pack current: %d\n", current);

	  /*
	   * 	While the current is below acceptable threshold
	   * 	put BQ into ship mode, MCU in low power mode and
	   * 	measure current every minute.
	   */
	  while(current < 100){
		  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);						//Reset BQ BOOT pin.
		  BQ76940_SHIPMODE();											//Enter BQ low power ship mode.
		  STM32L432KB_LP_STOP2();										//Enter MCU low power STOP2 mode. Implement this in function.
		  HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_1);						//Set BQ BOOT pin to enter normal mode.
		  current = BQ76940_get_HILO_register(CC_HI_REG, buf_current);	//Get current measurement.
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI
                              |RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x2000090E;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief LPUART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */

  /* USER CODE END LPUART1_Init 0 */

  /* USER CODE BEGIN LPUART1_Init 1 */

  /* USER CODE END LPUART1_Init 1 */
  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 209700;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_7B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPUART1_Init 2 */

  /* USER CODE END LPUART1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_4BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USB_OTG_FS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_FS_PCD_Init(void)
{

  /* USER CODE BEGIN USB_OTG_FS_Init 0 */

  /* USER CODE END USB_OTG_FS_Init 0 */

  /* USER CODE BEGIN USB_OTG_FS_Init 1 */

  /* USER CODE END USB_OTG_FS_Init 1 */
  hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
  hpcd_USB_OTG_FS.Init.dev_endpoints = 6;
  hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_OTG_FS.Init.Sof_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.battery_charging_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
  hpcd_USB_OTG_FS.Init.vbus_sensing_enable = ENABLE;
  if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_OTG_FS_Init 2 */

  /* USER CODE END USB_OTG_FS_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  HAL_PWREx_EnableVddIO2();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RST_UI_Pin|DC_UI_Pin|BOOT_BMS_Pin|SPI1_CS_UI_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD3_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_PowerSwitchOn_GPIO_Port, USB_PowerSwitchOn_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RST_UI_Pin DC_UI_Pin BOOT_BMS_Pin SPI1_CS_UI_Pin */
  GPIO_InitStruct.Pin = RST_UI_Pin|DC_UI_Pin|BOOT_BMS_Pin|SPI1_CS_UI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : BUSY_UI_Pin */
  GPIO_InitStruct.Pin = BUSY_UI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BUSY_UI_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ALERT_BMS_Pin */
  GPIO_InitStruct.Pin = ALERT_BMS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ALERT_BMS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD3_Pin LD2_Pin */
  GPIO_InitStruct.Pin = LD3_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_OverCurrent_Pin */
  GPIO_InitStruct.Pin = USB_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = USB_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 4 */
/**
  * @brief Set register values.
  * @param reg: the register address of register to be changed.
  * @param bit_order: bit value to set register to (example 0b01010101).
  * @retval None
  */
void BQ76940_set_register(uint8_t reg, uint8_t reg_val)
{
	ret = HAL_I2C_Mem_Write(&hi2c1, BQ76940_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &reg_val, 1, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		printf("ERROR on Writing to register\r\n");
	}
	printf("Register set");
}


/**
  * @brief Get register values.
  * @param reg: the register address of register to be read.
  * @param buffer: buffer to contain the register data.
  * @retval None
  */
void BQ76940_get_register(uint8_t reg, uint8_t* buffer)
{
	//Write BQ76940 cells according to cell_reg
	ret = HAL_I2C_Mem_Read(&hi2c1, BQ76940_ADDR, reg, I2C_MEMADD_SIZE_8BIT, buffer, 1, HAL_MAX_DELAY);
	if(ret != HAL_OK) {
		printf("ERROR on Reading from register\r\n");
	}
	printf("Register get");
}


/**
  * @brief Get HI/LO register values.
  * @param reg: the register address of HI register to be read.
  * @param buffer: buffer to contain the register data.
  * @retval None
  */
uint16_t BQ76940_get_HILO_register(uint8_t reg, uint8_t* buf)
{
	uint16_t val, temp_volt, temp_res, temp, current;
	ret = HAL_I2C_Mem_Read(&hi2c1, BQ76940_ADDR, reg, I2C_MEMADD_SIZE_8BIT, buf, 2, HAL_MAX_DELAY);
	if(ret != HAL_OK) {
		printf("ERROR on Reading from HILO register\r\n");
	} else {
		val = (((int16_t)buf[0] << 8) | buf[1]);

		//Switch case depending on the register to be read. This helps clean up main.
		switch(reg){
		case TS1_HI_REG:	//Case Temperature register
			temp_volt = val*(382*(10*exp(-6)));
			temp_res = ((10000 * temp_volt) / (3.3 - temp_volt));
			printf("Returning temp");
			val = ((-26.38*log(temp_res)) + 91.798);	//see excel sheet "NTC103AT_temp_res_correlation" in Oliver/3-Documentation/3-BMS_docs
			break;

		case CC_HI_REG:		//Case Current register
			if(val > 0x7FF){
				val |= 0xF000;
			}
			printf("Returning current");
			val = ((val*8.44)/(0.8*(10*exp(-3))));
			break;

		case 0:				//Case 'other' (Cell voltage register).
			printf("Returning cell voltage");
			break;
		}

		return val;

	}
	return 0;
}


/**
  * @brief Enter SHIP MODE.
  * @param None
  * @retval None
  */
void BQ76940_SHIPMODE()
{
	//Write to SYS_CTRL1 register SHUT_A = 0, SHUT_B = 1
	uint8_t buf01[8] = {0x01};
	ret = HAL_I2C_Mem_Write(&hi2c1, BQ76940_ADDR, 0x04, I2C_MEMADD_SIZE_8BIT, buf01, 1, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		printf("ERROR on writing to sys_ctrl1 [SHUT_A = 0, SHUT_B = 1]\r\n");
	}

	//Write to SYS_CTRL1 register SHUT_A = 1, SHUT_B = 0
	uint8_t buf10[8] = {0x02};
	ret = HAL_I2C_Mem_Write(&hi2c1, BQ76940_ADDR, 0x04, I2C_MEMADD_SIZE_8BIT, buf10, 1, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		printf("ERROR on writing to sys_ctrl1 [SHUT_A = 1, SHUT_B = 0]\r\n");
	}
}


void STM32L432KB_LP_STOP2()
{
	HAL_SuspendTick();
	HAL_RTCEx_SetWakeUpTimer(&hrtc, WakeUpTimer, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
	HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
	HAL_RTCEx_DeactivateWakeUpTimer(&hrtc);
	SystemClock_Config();
	HAL_ResumeTick();
}


/**
  * @brief  EXTI line detection callback.
  * @param  GPIO_Pin Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
// EXTI Line5 External Interrupt ISR Handler CallBackFun
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == GPIO_PIN_5) // If The INT Source Is EXTI Line5 (A5 Pin)
    {
    	uint8_t sys_buf[8];
    	BQ76940_get_register(SYS_STAT_REG, sys_buf);
    	for(int i = 0; i < sizeof(sys_buf); i++){
    		printf("%d", sys_buf[i]);
    	}
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
