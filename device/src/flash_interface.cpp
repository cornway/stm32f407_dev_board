#include "main.h"
#include "flash_diskio.h"

SPI_HandleTypeDef             SpiHandle;
AT_BYTE at_ll_init (void)
{
    GPIO_InitTypeDef  GPIO_InitStruct;
    __GPIOB_CLK_ENABLE();
    __GPIOG_CLK_ENABLE();
    
    
    
    GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_7 | GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
    
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13 | GPIO_PIN_7 | GPIO_PIN_8, GPIO_PIN_SET); /*wp, hold, -  deassert*/
    
    GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    
  __SPI1_CLK_ENABLE();
    	
  SpiHandle.Instance               = SPI1;
  SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
  SpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE;
  SpiHandle.Init.CLKPolarity       = SPI_POLARITY_LOW;
  SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
  SpiHandle.Init.CRCPolynomial     = 7;
  SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
  SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
  SpiHandle.Init.NSS               = SPI_NSS_SOFT;
  SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
  SpiHandle.Init.Mode              = SPI_MODE_MASTER;  
  if(HAL_SPI_Init(&SpiHandle) != HAL_OK)
  {
    /* Initialization Error */
    for (;;);
  }
  __HAL_SPI_ENABLE(&SpiHandle);
  return 0;
}
void at_ll_sel (AT_BYTE x)
{
    GPIO_PinState state = x == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET;
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, state);
}

uint8_t at_ll_get_sel ()
{
    return HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_13) ? false : true;
}


void at_ll_wp (AT_BYTE x)
{
	
}
void at_ll_hold (AT_BYTE x)
{
	
}

