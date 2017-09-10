#include "sensor_drv.h"
#include "tsc2046.h"
#include "gui.h"
#include "main.h"
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


drv_handle_t sensor_drv = {
    sensor_load,
    sensor_unload,
    sensor_probe,
    sensor_ioctl,
    sensor_io
};

#define NEXT(x) ((x) << 1)

enum {
    SENSOR_READ = 1,
    SENSOR_WRITE = NEXT(SENSOR_READ),
    SENSOR_CTL = NEXT(SENSOR_WRITE),
    SENSOR_ADD = NEXT(SENSOR_CTL),
    SENSOR_REM = NEXT(SENSOR_ADD),
    SENSOR_INV = NEXT(SENSOR_REM),
};

static TouchSensor *sensor_listeners[SENSOR_MAX_LISTENERS];

static drv_sword_t sensor_load (uint32_t a0, uint32_t a1)
{
    tsc2046.init(false);
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
    TouchPointTypeDef tp = *(TouchPointTypeDef *)e->getSource();
    int cause = e->getCause();
    switch (op) {
        case IOCTL_IRQ :
                tsc2046.tim_it_handle();
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
                        listener_id = get_match_listener((TouchSensor *)handler)
                        if (listener_id >= 0) {
                            sensor_listeners[listener_id] = NULL;
                        }
            break;
        case SENSOR_INV | SENSOR_CTL:
                        vm::lock(SENSOR_LOCK_ID);
                        for (int i = 0; i < SENSOR_MAX_LISTENERS; i++) {
                            if (sensor_listeners[i] != NULL) {
                                sensor_listeners[i]->setEvent(tp, cause);
                            }
                        }
                        vm::unlock(SENSOR_LOCK_ID);
            break;
        default : 
            break;
    }
    return 0;
}

static drv_sword_t sensor_io (void *, uint32_t, void *)
{
    
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
