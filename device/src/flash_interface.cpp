#include "main.h"
#include "flash_diskio.h"
#include "gpio.h"
#include "device.h"

gpio::gpio_dsc at_ll_sel_gpio = {GPIOG, GPIO_PIN_13};



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
  SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
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

static inline void
hal_spi_set_baud (void *__hspi, uint32_t baud)
{
    SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)__hspi;
    uint32_t reg = READ_REG(hspi->Instance->CR1) & ~SPI_BAUDRATEPRESCALER_256;
    WRITE_REG( hspi->Instance->CR1, (reg | baud) );
}


void at_ll_sel (AT_BYTE x)
{
    if (!x) {
        hal_spi_set_baud(&SpiHandle, SPI_BAUDRATEPRESCALER_2);
    } else {
        hal_spi_set_baud(&SpiHandle, SPI_BAUDRATEPRESCALER_16);
    }
    gpio::pin_set(at_ll_sel_gpio, x);
}
AT_BYTE at_ll_rw (AT_BYTE data)
{
    while ((SPI1->SR & SPI_FLAG_TXE) == 0){}
    *(__IO uint8_t *)&SPI1->DR = data;
    while ((SPI1->SR & SPI_FLAG_RXNE) == 0){}
    return *(__IO uint8_t *)&SPI1->DR;
}

void at_ll_wp (AT_BYTE x)
{
	
}
void at_ll_hold (AT_BYTE x)
{
	
}

