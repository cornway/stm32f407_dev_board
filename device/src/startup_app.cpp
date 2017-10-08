#include "startup_app.h"
#include "sensor_drv.h"
#include "device.h"
#include "time.h"
#include "it_vect.h"

enum {
    TFT_BSTATE_ON,
    TFT_BSTATE_UP,
    TFT_BSTATE_DOWN,
    TFT_BSTATE_OFF,
    TFT_BSTATE_IDLE
};

extern uint8_t is_usb_storage_operation ();

APPLICATION_CONTROL applicationControl;
uint16_t batteryValue = 0;
static uint16_t sleepTimeout = 0;
static int8_t tft_backlight_state = TFT_BSTATE_ON;

static void tft_bl_event ();
static void tft_bl_task ();

FontArray fontArray;
static TouchSensor touch_sensor;

INT32_T VM_SYS_THREAD (WORD_T size, void *arg)
{
    time::delay_ms(200);
    control_init ();
    tft.init();
    sleepTimeout = 0;
    
    init_adc_bat();
    sensor_drv.param[0] = TFT_WIDTH;
    sensor_drv.param[1] = TFT_HEIGHT;
    touch_sensor.setId( vm::drv_link(&sensor_drv, TSC_TIM_IRQn, 0).ERROR);
    vm::drv_ctl(touch_sensor.getId(), SENSOR_CTL | SENSOR_ADD, (uint32_t)&touch_sensor);
    
    THREAD_HANDLE th;
    th.Arg = 0;
    th.argSize = 0;
    th.Callback = (void *)render_app;
    th.Name = "render";
    th.Priority = 4;
    th.StackSize = 512;
    ARG_STRUCT_T ret = vm::create(&th);
    th.Callback = (void *)main_app;
    th.Name = "main";
    th.Priority = 5;
    th.StackSize = 6144;
    ret = vm::create(&th);
    uint32_t err = 0;

    //err = load_program(program_space_begin, "img.img");
    if (err == 0) {
        //err = join_program(program_space_begin, "background", 0, NULL);
    }
    touch_sensor.addListener([](abstract::Event e) -> void {
            if (e.getCause() == SENSOR_RELEASE) {
                tft_bl_event();
            } else {
                if ((e.getCause() == SENSOR_CLICK)) {
                    if (applicationControl.audioControl.soundOn == SOUND_ON) {
                        if (clickWave != nullptr) {
                            clickWave->play();
                        }
                    }
                }
            }
            sleepTimeout = 0;
    });
    for (;;) {
        if (!is_usb_storage_operation()) {
            vm::drv_ctl(touch_sensor.getId(), SENSOR_CTL | SENSOR_INV, 0);
        }
        touch_sensor.invokeEvent();
        tft_bl_task();
        if (tft_backlight_state == TFT_BSTATE_OFF) {
            applicationControl.powerControl.powerConsumption = POWER_CONSUMPTION_SLEEP;
        } else {
            batteryValue = measureBattery();
        }
        vm::yield();
    }
}

static WaveSample *sample = nullptr;;
void updateAudio (WaveSample **s, char *path)
{
    sample = *s;
    *s = nullptr;
    FIL *file = new FIL;
    if (file == nullptr) {
        *s = sample;
        return;
    }
    WAVE *wave = new WAVE(path);
    if (wave == nullptr) {
        delete file;
        *s = sample;
        return;
    }
    if (wave->open(file) != WAVE_OK) {
        delete file;
        delete wave;
        *s = sample;
        return;
    }
    WAVE_DSC waveDsc = wave->getInfo();
    uint8_t *buffer = new uint8_t[waveDsc.dataSize];
    if (buffer == nullptr) {
        delete file;
        delete wave;
        *s = sample;
        return;
    }
    *s = new WaveSample(path, waveDsc, buffer);
    if (*s == nullptr) {
        delete file;
        delete wave;
        delete buffer;
        *s = sample;
        return;
    }
    if (sample != nullptr) {
        delete sample->getBuffer();
        delete sample;
    }
    wave->loadSample(file, buffer);
    delete wave;
    delete file;
}

void appOnActionHook ()
{
    if (applicationControl.audioControl.soundOn == SOUND_OFF) {
        return;
    }
    time::delay_ms(200);
    if (confirmWave != nullptr) {
        confirmWave->play();
    }
}

static void control_init ()
{
    applicationControl.audioControl.soundOn                 = SOUND_OFF;
    applicationControl.graphicControl.quality               = GRAPHIC_QUALITY_BEST;
    applicationControl.graphicControl.acceleration          = GRAPHIC_ACCELERATION_ON;
    applicationControl.powerControl.powerConsumption        = POWER_CONSUMPTION_FULL;
    applicationControl.tftControl.backlightLock             = 0;
    applicationControl.tftControl.backlight                 = TFT_BACKLIGHT_ON;
    applicationControl.tftControl.backlightValue            = 100;
    applicationControl.tftControl.defaultBacklightValue     = BRIGHTNESS_MAX_LEVEL;
    applicationControl.sleepTimeout                         = 2000;
    applicationControl.snapshotBpp                          = 1;
    
    strcpy(applicationControl.networkControl.ipv4Str, "192.168.0.1");
    strcpy(applicationControl.networkControl.macStr, "00.00.00.00.00.00"); 
    
    update_ipv4("192.168.0.1", applicationControl.networkControl.ipv4_ip);
    update_mac("00.00.00.00.00.00", applicationControl.networkControl.mac);
}



#include "stm32f4xx_hal.h"

ADC_HandleTypeDef    AdcHandle;

static void init_adc_bat ()
{
  __ADC1_CLK_ENABLE();
    
  ADC_ChannelConfTypeDef sConfig;
  AdcHandle.Instance                   = ADC1;
  
  AdcHandle.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV2;
  AdcHandle.Init.Resolution            = ADC_RESOLUTION_12B;
  AdcHandle.Init.ScanConvMode          = DISABLE;
  AdcHandle.Init.ContinuousConvMode    = DISABLE;
  AdcHandle.Init.DiscontinuousConvMode = DISABLE;
  AdcHandle.Init.NbrOfDiscConversion   = 0;
  AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
  AdcHandle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;
  AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  AdcHandle.Init.NbrOfConversion       = 1;
  AdcHandle.Init.DMAContinuousRequests = DISABLE;
  AdcHandle.Init.EOCSelection          = DISABLE;
      
  if(HAL_ADC_Init(&AdcHandle) != HAL_OK)
  {
    /* Initialization Error */
    for (;;);
  }
  
  /*##-2- Configure ADC regular channel ######################################*/  
  sConfig.Channel      = ADC_CHANNEL_VBAT;
  sConfig.Rank         = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_28CYCLES;
  sConfig.Offset       = 0;
  
  if(HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
  {
    /* Channel Configuration Error */
    for(;;);
  }
}

static uint16_t measureBattery ()
{
  uint16_t value = 0;
  if(HAL_ADC_Start(&AdcHandle) != HAL_OK)
  {
    for(;;);
  }
  HAL_ADC_PollForConversion(&AdcHandle, 10);
  value = HAL_ADC_GetValue(&AdcHandle);
  
  return value;
}

static void tft_bl_event ()
{
    switch (tft_backlight_state) {
        case TFT_BSTATE_DOWN:
        case TFT_BSTATE_OFF:
        case TFT_BSTATE_IDLE:
            tft_backlight_state = TFT_BSTATE_UP;
            applicationControl.powerControl.powerConsumption = POWER_CONSUMPTION_FULL;
            applicationControl.tftControl.backlight = TFT_BACKLIGHT_ON;
            vm::fire_event("wakeup");
            break;
        default :
            break;
    }
}

static void tft_bl_task ()
{
    static uint16_t ticks = 0;
    switch (tft_backlight_state) {
        case TFT_BSTATE_ON:
            if (++sleepTimeout >= applicationControl.sleepTimeout) {
                tft_backlight_state = TFT_BSTATE_DOWN;
            }
            break;
        case TFT_BSTATE_DOWN:
            if (applicationControl.tftControl.backlightValue) {
                if (++ticks  % 10 == 0)
                    applicationControl.tftControl.backlightValue --;
            } else {
                tft_backlight_state = TFT_BSTATE_OFF;
            }
            break;
        case TFT_BSTATE_UP:
            if (applicationControl.tftControl.backlightValue <= 100) {
                applicationControl.tftControl.backlightValue++;
            } else {
                tft_backlight_state = TFT_BSTATE_ON;
            }
            break;
        case TFT_BSTATE_OFF:
            if (applicationControl.tftControl.backlightLock == 0) {
                applicationControl.tftControl.backlight = TFT_BACKLIGHT_OFF;
            }
            tft_backlight_state = TFT_BSTATE_IDLE;
            break;
        case TFT_BSTATE_IDLE:
            break;
        default:
            break;
    };

}

