#include <stdint.h>
#include "main.h"
#include "it_vect.h"
#include "gpio.h"
#include "device.h"
#include "device_conf.h"
#include "app_def.h"

#include "VM.h"
#include "vmapi.h"
#include "vmem.h"

extern "C" void *vmalloc (UINT_T size)
{
    return malloc(size);
}
extern "C" void vmfree (void *p)
{
    free(p);
}


#include "init.h"
#include "fpga.h"
#include "ccpu.h"
#include "tsc2046.h"
#include "adv7180.h"
#include "adv7180_conf.h"

ADV7180_VIDEO_DECODER adv7180;
Fpga fpga;

Device::Device ()
{
    
}


#include "flash_diskio.h"
#include "ff_gen_drv.h"

extern Diskio_drvTypeDef  FLASHDISK_Driver;

FATFS FLASHDISKFatFs; 
char FLASHDISKPath[4]; 

void Device::init ()
{
    init::hal();
    init::clock();
    init::clock_enable();
    init::nvic ();
    init::tim_hal_milis();
    init::fsmc();
    init::clock_out();
    init::gpio();
    init::tim_tsc_update();
    init::audio();
    init::usb();
    init::i2c1();
    adv7180.init(ADV_I2C_ADDRESS);
    
    fpga.init(0);
    uint16_t psram_lat =    (0 << CCPU_PSRAM_ADLAT_GP) 	| 
                            (1 << CCPU_PSRAM_DLAT_GP) 	| 
							(0 << CCPU_PSRAM_HOLD_GP);
    
    CCPU_WRITE_REG(CCPU_WRITE_PSRAM_LAT, psram_lat);
    CCPU_WRITE_REG(CCPU_WRITE_RENDER_CTL_REGISTER, 0);
    CCPU_WRITE_REG(CCPU_WRITE_FILL_CTL, 0);
    CCPU_WRITE_REG(CCPU_WRITE_COPY_CTL, 0);
    CCPU_WRITE_REG(CCPU_WRITE_COLOR_MODE, 0);
    
    FLASHDISK_Driver.disk_initialize(1);
    FATFS_LinkDriver(&FLASHDISK_Driver, FLASHDISKPath);
    f_mount(&FLASHDISKFatFs, (TCHAR const*)FLASHDISKPath, 0);
    
    VMINIT();
    HAL_InitTick (TICK_INT_PRIORITY);
}


void Device::run ()
{ 
        VMBOOT();
        for (;;) {
        }
}


uint32_t sys_time_milis = 0; /*for lwip tcp\ip stack*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	
	switch ((uint32_t)htim->Instance) {
		case (uint32_t)TIM3: 
			         
		break;
		case (uint32_t)TIM2:
                     sys_time_milis++;
			         HAL_IncTick();           
		break;
            default:
                  break;
	}
}


void *operator new (uint32_t size)
{
    return malloc(size);
}

void operator delete (void *p)
{
    free(p);
}


extern gpio::gpio_dsc tsc_pen_pin_dsc;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    /*
    if (GPIO_Pin == tsc_pen_pin_dsc.pin) {
        tsc2046.IT_Handle();
    }
    */
}

#include "gui_defs.h"
void bmp_mach_conv (void **dest, uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    uint16_t *p = (uint16_t *)*dest;
    *p = RGB24_TO_RGB16(r, g, b);
    *dest = (void *)++p;
}

#include "fgpu.h"

extern gpio::gpio_dsc FpgaMax10_gpu_wait_pin;
void gpu_wait (void)
{
    while (gpio::pin_read(FpgaMax10_gpu_wait_pin) == true) {}
}


#include "wave_sample.h"

extern DAC_HandleTypeDef    dac1Handle;

void WaveSample::ll_play (WAVE_DSC dsc, void *buffer)
{
  uint32_t allign = 0;
  switch (dsc.bitRate) {
      case 8 : allign = DAC_ALIGN_8B_R;
          break;
      case 16 : allign = DAC_ALIGN_12B_L;
          break;
      default : return;
          //break;
  }
  if(HAL_DAC_Start_DMA(&dac1Handle, DAC_CHANNEL_1, (uint32_t*)buffer, dsc.dataSize, allign) != HAL_OK)
  {
    for (;;);
  }
}
extern uint32_t load_program (void *memory, const char *path)
{
    vm::Mutex __mem(MEMORY_ALLOC_LOCK_ID);
    vm::Mutex __fs(FILE_SYSTEM_LOCK_ID);
    FIL *file =  (FIL *)vmalloc(sizeof(FIL));
    if (file == NULL) {
        return 1;
    }
    vm::Cleanup __fs_cleanup(file);
    uint32_t res = 0;
    res = f_open(file, path, FA_READ);
    if(res  != FR_OK ) {
        return 1;
    }
    uint8_t buf[256];
    uint32_t f_bytes_read = 0;
    uint8_t *dest = (uint8_t *)memory;
    do  {
       res = f_read(file, buf, 256, &f_bytes_read);
       memcpy(dest, buf, f_bytes_read);
       dest += f_bytes_read;
     } while ((res = FR_OK));
    f_close(file);
    return 0;
}

extern uint32_t join_program (void *mem, const char *name,  int c, char **v)
{
        THREAD_HANDLE th;
        th.Arg = v;
        th.argSize = c;
        th.Callback = mem;
        th.Name = name;
        th.Priority = 2;
        th.StackSize = 4096;
        return vm::create(&th).ERROR;
}


#include "time.h"
void WaveSample::ll_wait ()
{
    time::delay_ms(200);
}


volatile static uint8_t spi1_lock = 0;
AT_BYTE at_ll_is_busy (void)
{
    return spi1_lock;
}

AT_BYTE at_ll_rw (AT_BYTE write_data)
{
    if (spi1_lock) {
        return 0;
    }
    spi1_lock = 1;
    while ((SPI1->SR & SPI_FLAG_TXE) == 0){}
			*(__IO uint8_t *)&SPI1->DR = write_data;
	while ((SPI1->SR & SPI_FLAG_RXNE) == 0){}
    spi1_lock = 0;
    return *(__IO uint8_t *)&SPI1->DR;
}

extern gpio::gpio_dsc tsc_busy_pin_dsc;
bool tsc2046Drv::ll_busy ()
{
    return gpio::pin_read(tsc_busy_pin_dsc) | spi1_lock;
}

uint8_t tsc2046Drv::ll_rw (uint8_t data)
{
    if (spi1_lock) {
            return 0;
    }
    spi1_lock = 1;
    while ((SPI1->SR & SPI_FLAG_TXE) == 0){}
			*(__IO uint8_t *)&SPI1->DR = data;
	while ((SPI1->SR & SPI_FLAG_RXNE) == 0){}
    spi1_lock = 0;
    return *(__IO uint8_t *)&SPI1->DR;
}



#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_msc.h"
#include "usbd_storage.h"

void *usbd_malloc (uint32_t size)
{
    return malloc(size);
}

void usbd_free (void *p)
{
    free(p);
}


extern USBD_HandleTypeDef USBD_Device;
void usbdStart (void)
{
    USBD_Start(&USBD_Device);
}

void usbdStop (void)
{
    USBD_Stop(&USBD_Device);
}






