#include "sensor_drv.h"
#include "tsc2046.h"
#include "device_conf.h"
#include "abstract.h"

static tsc2046Drv tsc2046;

static int get_avail_listener();
static TouchSensor *get_ready_listener();
static int32_t get_match_listener(TouchSensor *);

static drv_sword_t sensor_load (uint32_t, uint32_t);
static drv_sword_t sensor_unload (uint32_t);
static drv_sword_t sensor_probe (uint32_t);
static drv_sword_t sensor_ioctl (void *, uint32_t, void *);
static drv_sword_t sensor_io (void *, uint32_t, void *);

static int32_t width = 0;
static int32_t height = 0;

drv_handle_t sensor_drv = {
    sensor_load,
    sensor_unload,
    sensor_probe,
    sensor_ioctl,
    sensor_io,
    "input0",
};

static TouchSensor *sensor_listeners[SENSOR_MAX_LISTENERS];

static drv_sword_t sensor_load (uint32_t a0, uint32_t a1)
{
    tsc2046.init(false);
    tsc2046.addListener([](abstract::Event e) -> void {
        for (int i = 0; i < SENSOR_MAX_LISTENERS; i++) {
            if (sensor_listeners[i] != NULL) {
                sensor_listeners[i]->setEvent((void *)e.getSource(), e.getCause());
            }
        }
    });
}

static drv_sword_t sensor_unload (uint32_t a0)
{
    
}

static drv_sword_t sensor_probe (uint32_t a0)
{
    
}

static drv_sword_t sensor_ioctl (void *handler, uint32_t op, void *p)
{
    int32_t listener_id = 0;
    abstract::Event *e = (abstract::Event *)p;
    TouchPointTypeDef *tp = (TouchPointTypeDef *)e->getSource();
    int cause = e->getCause();
    switch (op) {
        case IOCTL_IRQ :
                tsc2046.tim_it_handle();
                tsc2046.invoke(tp, width, height);
            break;
        case IOCTL_DMA :
            break;
        case SENSOR_WRITE :
            break;
        case SENSOR_ADD | SENSOR_CTL: 
                        listener_id = get_avail_listener();
                        sensor_listeners[listener_id] = (TouchSensor *)handler;
            break;
        case SENSOR_REM | SENSOR_CTL:
                        listener_id = get_match_listener((TouchSensor *)handler);
                        if (listener_id >= 0) {
                            sensor_listeners[listener_id] = NULL;
                        }
            break;
        case SENSOR_INV | SENSOR_CTL:
                        
            break;
        default : 
            break;
    }
    return 0;
}

static drv_sword_t sensor_io (void *, uint32_t, void *)
{
    return 0;
}

static int get_avail_listener()
{
    TouchSensor *ts = NULL;
    for (uint8_t i = 0; i < SENSOR_MAX_LISTENERS; i++) {
        ts = sensor_listeners[i];
        if (ts == NULL) {
            return i;
        }
    }
    return -1;
}

static TouchSensor *get_ready_listener()
{
    TouchSensor *ts = NULL;
    for (uint8_t i = 0; i < SENSOR_MAX_LISTENERS; i++) {
        ts = sensor_listeners[i];
        if (ts != NULL) {
            return ts;
        }
    }
    return NULL;
}

static int32_t get_match_listener(TouchSensor *ts)
{
    for (uint8_t i = 0; i < SENSOR_MAX_LISTENERS; i++) {
        if (ts == sensor_listeners[i]) {
            return i;
        }
    }
    return -1;
}

int32_t sensor_hal_init (int32_t w, int32_t h)
{
    int tsc2046_cal_res = TSC2046_CAL_FILE_NOT_FOUND;
    tsc2046_cal_res = tsc2046.loadCalData((char *)"tsc2046.cal");
    if (tsc2046_cal_res == TSC2046_CAL_FILE_NOT_FOUND) {
        tsc2046_cal_res = tsc2046.calibration((char *)"tsc2046.cal");
    }
    if (tsc2046_cal_res != TSC2046_CAL_OK) {
        return -1;
    }
    width = w;
    height = h;
    return 0;
}
