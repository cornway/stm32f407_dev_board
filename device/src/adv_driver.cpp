#include "cam_app.h"
#include "explorer_app.h"
#include "fgpu.h"
#include "ccpu.h"
#include "camera.h"
#include "adv7180_conf.h"
#include "device_conf.h"
#include "time.h"
#include "adv_driver.h"

static drv_sword_t adv_load (uint32_t, uint32_t);
static drv_sword_t adv_unload (uint32_t);
static drv_sword_t adv_probe (uint32_t);
static drv_sword_t adv_ioctl (void *, void *, void *);
static drv_sword_t adv_io (void *, uint32_t, void *);


drv_handle_t adv_driver = 
{
        adv_load, 
        adv_unload,
        adv_probe,
        adv_ioctl,
        adv_io,
        {0, 0, 0, 0},
        "adv0",
};

ADV7180_VIDEO_DECODER decoder;


static drv_sword_t adv_load (uint32_t addr, uint32_t data)
{
    if (decoder.init(addr) < 0) {
        return 1;
    }
    decoder.powerUp();
    time::delay_ms(50);
    
    if (DCMIinit() < 0) {
        DCMIDeInit();
        return 1;
    }
    return 0;
} 
static drv_sword_t adv_unload (uint32_t)
{
    decoder.powerDown();
    DCMIDeInit();
}
static drv_sword_t adv_probe (uint32_t)
{

}
static drv_sword_t adv_ioctl (void *, void *, void *)
{

}
static drv_sword_t adv_io (void *, uint32_t, void *)
{

}





