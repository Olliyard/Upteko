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
I2C_HandleTypeDef hi2c3;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN PV */
static const uint8_t BQ76940_ADDR = 0x08<<1;//BMS temp address
static const uint8_t SYS_STAT_REG = 0x00;	//SYS_STAT register
static const uint8_t SYS_CTRL1_REG = 0x04;	//SYS_CTRL1 register
static const uint8_t SYS_CTRL2_REG = 0x05;	//SYS_CTRL2 register
static const uint8_t VC1_HI_REG = 0x0C;	//Needed to start the for() sequence later.
static const uint8_t VC1_LO_REG = 0x0D;	//Needed to start the for() sequence later.
//static const uint8_t VC2_HI = 0x0E;
//static const uint8_t VC2_LO = 0x0F;
//static const uint8_t VC3_HI = 0x10;
//static const uint8_t VC3_LO = 0x11;
//static const uint8_t VC5_HI = 0x14;
//static const uint8_t VC5_LO = 0x15;
//static const uint8_t VC6_HI = 0x16;
//static const uint8_t VC6_LO = 0x17;
//static const uint8_t VC7_HI = 0x18;
//static const uint8_t VC7_LO = 0x19;
//static const uint8_t VC8_HI = 0x1A;
//static const uint8_t VC8_LO = 0x1B;
//static const uint8_t VC10_HI = 0x1E;
//static const uint8_t VC10_LO = 0x1F;
//static const uint8_t VC11_HI = 0x20;
//static const uint8_t VC11_LO = 0x21;
//static const uint8_t VC12_HI = 0x22;
//static const uint8_t VC12_LO = 0x23;
//static const uint8_t VC13_HI = 0x24;
//static const uint8_t VC13_LO = 0x25;
//static const uint8_t VC15_HI = 0x28;
//static const uint8_t VC15_LO = 0x29;
static const uint8_t TS1_HI_REG = 0x2C;		//TS1 register
static const uint8_t TS1_LO_REG = 0x2D;		//TS1 register
static const uint8_t CC_HI_REG = 0x32;		//CC register
static const uint8_t CC_LO_REG = 0x33;		//CC register
static const uint8_t ADCGAIN_REG = 0x50; 	//ADC gain register
static const uint8_t ADCOFFSET_REG = 0x51; 	//ADC offset register

HAL_StatusTypeDef ret;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C3_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */
float BQ76940_get_cc_current(uint8_t cc_reg_hi, uint8_t cc_reg_lo);
int BQ76940_get_cell_voltage(uint8_t cell_reg_hi, uint8_t cell_reg_lo);
float BQ76940_get_temp(uint8_t temp_reg_hi, uint8_t temp_reg_lo);
void BQ76940_set_register(uint8_t reg, uint8_t reg_val);
void BQ76940_get_register(uint8_t reg, uint8_t* buf);
void BQ76940_get_HILO_register(uint8_t reg, uint8_t* buf);
void BQ76940_SHIPMODE(uint8_t reg1);
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
	uint8_t buf_ADC_gain[8], buf_ADC_offset[8], buf_cell_volt[64];
//	uint16_t temp_res, temp;
//	float pack_current;
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
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_I2C3_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */

  //Function needed for checking fail-state registers.
  BQ76940_get_register(ADCGAIN_REG, buf_ADC_gain);
  BQ76940_get_register(ADCOFFSET_REG, buf_ADC_offset);
  BQ76940_set_register(SYS_CTRL1_REG, 0x08);	//Enable temperature measurements (TEMP_SEL = 1)
  BQ76940_set_register(SYS_CTRL2_REG, 0x40);	//Enable CC ALWAYS ON mode (CC_EN = 1)
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);	//Set SPI CS pin high
  EPD_2IN66_Init();	//Initialize the E-paper display
  EPD_2IN66_Clear();
  buf_ADC_gain[0] = (buf_ADC_gain[0]*(10*exp(-6)));
  buf_ADC_offset[0] = (buf_ADC_offset[0]*(10*exp(-3)));

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  //Get each cell reading from their respective registers and store the voltage according to ADC gain and offset.
	  for(size_t i = 0; i<16; i++)
	  {
		  buf_cell_volt[i] = BQ76940_get_cell_voltage(VC1_HI_REG + (i*2), VC1_LO_REG + (i*2));
		  buf_cell_volt[i] = ((buf_ADC_gain[0] * buf_cell_volt[i])+buf_ADC_offset[0]);
	  }

	  //calculate pack total voltage from individual cell voltages
	  for(size_t i = 0; i<15; i++)
	  {
		  pack_voltage += buf_cell_volt[i];
	  }

	  //get the equivalent temperature resistance value
	  uint16_t temp_res = BQ76940_get_temp(TS1_HI_REG, TS1_LO_REG);
	  uint16_t temp = ((-26.38*log(temp_res)) + 91.798);	//see excel sheet "NTC103AT_temp_res_correlation" in Oliver/3-Documentation/3-BMS_docs
	  printf("temp: %d\n", temp);

	  float pack_current = BQ76940_get_cc_current(CC_HI_REG, CC_LO_REG);
	  printf("pack current: %d", (uint16_t)pack_current);
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
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
  hi2c1.Init.Timing = 0x10909CEC;
  hi2c1.Init.OwnAddress1 = 16;
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
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.Timing = 0x10909CEC;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

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
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, RST_UI_Pin|DC_UI_Pin|SPI1_CS_UI_Pin|BOOT_BMS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED1_STATUS_Pin|LED2_STATUS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : TOGGLE_UI_Pin BUSY_UI_Pin */
  GPIO_InitStruct.Pin = TOGGLE_UI_Pin|BUSY_UI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : RST_UI_Pin DC_UI_Pin SPI1_CS_UI_Pin BOOT_BMS_Pin */
  GPIO_InitStruct.Pin = RST_UI_Pin|DC_UI_Pin|SPI1_CS_UI_Pin|BOOT_BMS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LED1_STATUS_Pin LED2_STATUS_Pin */
  GPIO_InitStruct.Pin = LED1_STATUS_Pin|LED2_STATUS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : ALERT_BMS_Pin */
  GPIO_InitStruct.Pin = ALERT_BMS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ALERT_BMS_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
/**
 * @brief Get current measurements from pack
 * @param cc_reg1: coulomb counter register HI.
 * @param cc_reg2: coulomb counter register LO.
 * @retval float: the
 */
float BQ76940_get_cc_current(uint8_t cc_reg1, uint8_t cc_reg2)
{
	uint16_t val;
	uint8_t buf_cc1[14];
	uint8_t buf_cc2[14];

	//Write BQ76940 Coulomb Counter HI register
	ret = HAL_I2C_Master_Transmit(&hi2c1, BQ76940_ADDR, &cc_reg1, 1, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		strcpy((char*)buf_cc1, "ERROR on TX\r\n");
	} else {

		//Read BQ76940 Coulomb Counter HI register
		ret = HAL_I2C_Master_Receive(&hi2c1, BQ76940_ADDR, buf_cc1, 1, HAL_MAX_DELAY);
		if(ret != HAL_OK){
			strcpy((char*)buf_cc1, "ERROR on RX\r\n");
		}
	}

	//Write BQ76940 Coulomb Counter LO register
	ret = HAL_I2C_Master_Transmit(&hi2c1, BQ76940_ADDR, &cc_reg2, 1, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		strcpy((char*)buf_cc2, "ERROR on TX\r\n");
	} else {

		//Read BQ76940 Coulomb Counter LO register
		ret = HAL_I2C_Master_Receive(&hi2c1, BQ76940_ADDR, buf_cc2, 1, HAL_MAX_DELAY);
		if(ret != HAL_OK){
			strcpy((char*)buf_cc2, "ERROR on RX\r\n");
		} else {
			val = (((uint16_t)buf_cc1[0]<<8) + buf_cc2[0]);
				// Convert to 2's complement, since temperature can be negative
			if ( val > 0x7FF ) {
				val |= 0xF000;
			}
			return ((val*8.44)/(0.8*(10*exp(-3))));
		}
		return 0;
	}
	return 0;
}


/**
  * @brief Get cell values.
  * @param temp_reg_hi: the HI VCx register address.
  * @param temp_reg_lo: the LO VCx register address.
  * @retval float: the cell voltage in mV.
  */
int BQ76940_get_cell_voltage(uint8_t cell_reg_hi, uint8_t cell_reg_lo)
{
	uint16_t val;
	uint8_t buf_cell_hi[14];
	uint8_t buf_cell_lo[14];

	//Write BQ76940 Cell HI register
	ret = HAL_I2C_Master_Transmit(&hi2c1, BQ76940_ADDR, &cell_reg_hi, 1, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		strcpy((char*)buf_cell_hi, "ERROR on TX\r\n");
	} else {

		//Read BQ76940 Cell HI register
		ret = HAL_I2C_Master_Receive(&hi2c1, BQ76940_ADDR, buf_cell_hi, 1, HAL_MAX_DELAY);
		if(ret != HAL_OK){
			strcpy((char*)buf_cell_hi, "ERROR on RX\r\n");
		}
	}

	//Write BQ76940 Cell LO register
	ret = HAL_I2C_Master_Transmit(&hi2c1, BQ76940_ADDR, &cell_reg_lo, 1, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		strcpy((char*)buf_cell_lo, "ERROR on TX\r\n");
	} else {

		//Read BQ76940 Cell LO register
		ret = HAL_I2C_Master_Receive(&hi2c1, BQ76940_ADDR, buf_cell_lo, 1, HAL_MAX_DELAY);
		if(ret != HAL_OK){
			strcpy((char*)buf_cell_lo, "ERROR on RX\r\n");
		}
	}

	val = (((uint16_t)buf_cell_hi[0]<<8) + buf_cell_lo[0]);
	return val;
}


/**
  * @brief Get temperature register values.
  * @param temp_reg_hi: the HI TS register address.
  * @param temp_reg_lo: the LO TS register address.
  * @retval float: the temperature equivalent resistance.
  */
float BQ76940_get_temp(uint8_t temp_reg_hi, uint8_t temp_reg_lo)
{
	uint16_t val_volt, val_res;
	uint8_t buf_temp_hi[14];
	uint8_t buf_temp_lo[14];

	//Write BQ76940 temp HI register
	ret = HAL_I2C_Master_Transmit(&hi2c1, BQ76940_ADDR, &temp_reg_hi, 1, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		strcpy((char*)buf_temp_hi, "ERROR on TX\r\n");
	} else {

		//Read BQ76940 temp HI register
		ret = HAL_I2C_Master_Receive(&hi2c1, BQ76940_ADDR, buf_temp_hi, 1, HAL_MAX_DELAY);
		if(ret != HAL_OK){
			strcpy((char*)buf_temp_hi, "ERROR on RX\r\n");
		}
	}

	//Write BQ76940 temp LO register
	ret = HAL_I2C_Master_Transmit(&hi2c1, BQ76940_ADDR, &temp_reg_lo, 1, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		strcpy((char*)buf_temp_lo, "ERROR on TX\r\n");
	} else {

		//Read BQ76940 temp LO register
		ret = HAL_I2C_Master_Receive(&hi2c1, BQ76940_ADDR, buf_temp_lo, 1, HAL_MAX_DELAY);
		if(ret != HAL_OK){
			strcpy((char*)buf_temp_lo, "ERROR on RX\r\n");
		}
	}

	val_volt = (((uint16_t)buf_temp_hi[0]<<8) + buf_temp_lo[0]);
	val_volt = val_volt*(382*(10*exp(-6)));
	val_res = ((10000 * val_volt) / (3.3 - val_volt));

	return val_res;
}


/**
  * @brief Set register values.
  * @param reg: the register address of register to be changed.
  * @param bit_order: bit value to set register to (example 0b01010101).
  * @retval None
  */
void BQ76940_set_register(uint8_t reg, uint8_t reg_val)
{
	uint8_t buf[14];
	buf[0] = reg;
	buf[1] = reg_val;
	ret = HAL_I2C_Master_Transmit(&hi2c1, BQ76940_ADDR, buf, 2, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		strcpy((char*)buf, "ERROR on TX\r\n");
	}
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
	ret = HAL_I2C_Master_Transmit(&hi2c1, BQ76940_ADDR, &reg, 1, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		strcpy((char*)buffer, "ERROR on TX\r\n");
	} else {

		//Read BQ76940 cells according to cellbal_reg
		ret = HAL_I2C_Master_Receive(&hi2c1, BQ76940_ADDR, buffer, 1, HAL_MAX_DELAY);
		if(ret != HAL_OK){
			strcpy((char*)buffer, "ERROR on RX\r\n");
		}
	}
}


/**
  * @brief Get HI/LO register values.
  * @param reg: the register address of HI register to be read.
  * @param buffer: buffer to contain the register data.
  * @retval None
  */
void BQ76940_get_HILO_register(uint8_t reg, uint8_t* buf)
{
    ret = HAL_I2C_Master_Transmit(&hi2c1, BQ76940_ADDR, &reg, 1, HAL_MAX_DELAY);
    if(ret != HAL_OK) {
    	strcpy((char*)buf, "ERROR on Tx\r\n");
    } else {

      // Read 2 bytes from the temperature register
      ret = HAL_I2C_Master_Receive(&hi2c1, BQ76940_ADDR, buf, 2, HAL_MAX_DELAY);
      if ( ret != HAL_OK ) {
        strcpy((char*)buf, "ERROR on Rx\r\n");
      } else {

        //Combine the bytes
        buf[2] = ((int16_t)buf[0] << 8) | buf[1];
      }
    }
}



/**
  * @brief Enter SHIP MODE.
  * @param reg: the register address of register SYS_CTRL1.
  * @retval None
  */
void BQ76940_SHIPMODE(uint8_t reg1)
{
	uint8_t buf[14];

	//Write to SYS_CTRL1 register SHUT_A = 0, SHUT_B = 1
	buf[0] = reg1;
	buf[1] = 0x01;
	ret = HAL_I2C_Master_Transmit(&hi2c1, BQ76940_ADDR, buf, 2, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		strcpy((char*)buf, "ERROR on TX\r\n");
	}

	//Write to SYS_CTRL1 register SHUT_A = 1, SHUT_B = 0
	buf[0] = reg1;
	buf[1] = 0x02;
	ret = HAL_I2C_Master_Transmit(&hi2c1, BQ76940_ADDR, buf, 2, HAL_MAX_DELAY);
	if(ret != HAL_OK){
		strcpy((char*)buf, "ERROR on TX\r\n");
	}
}


// EXTI Line9 External Interrupt ISR Handler CallBackFun
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == GPIO_PIN_8) // If The INT Source Is EXTI Line8 (A8 Pin)
    {
    	uint8_t sys_buf[8];
    	BQ76940_get_register(SYS_STAT_REG, sys_buf);
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
