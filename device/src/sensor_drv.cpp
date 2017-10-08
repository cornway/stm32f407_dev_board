#include "sensor_drv.h"
#include "tsc2046.h"
#include "device_conf.h"
#include "abstract.h"
#include "vmapi.h"
#include "app_def.h"

static tsc2046Drv tsc2046;

static int get_avail_listener();
static TouchSensor *get_ready_listener();
static int32_t get_match_listener(TouchSensor *);

static drv_sword_t sensor_load (uint32_t, uint32_t);
static drv_sword_t sensor_unload (uint32_t);
static drv_sword_t sensor_probe (uint32_t);
static drv_sword_t sensor_ioctl (void *, void *, void *);
static drv_sword_t sensor_io (void *, uint32_t, void *);

static INT32_T join_sensor_task (WORD_T size, void *argv);
static INT32_T sensor_task (WORD_T size, void *argv);

static int32_t width = 0;
static int32_t height = 0;

typedef struct  {
    WORD_T a, b;
} pair_t;

drv_handle_t sensor_drv = {
    sensor_load,
    sensor_unload,
    sensor_probe,
    sensor_ioctl,
    sensor_io,
    {0, 0, 0, 0},
    "input0",
};

static TouchSensor *sensor_listeners[SENSOR_MAX_LISTENERS];

static drv_sword_t sensor_load (uint32_t a0, uint32_t a1)
{
    pair_t p = {sensor_drv.param[0], sensor_drv.param[1]};
    join_sensor_task(0, &p);
    return 0;
}

static drv_sword_t sensor_unload (uint32_t a0)
{
    return 0;
}

static drv_sword_t sensor_probe (uint32_t a0)
{
    return 0;
}

static drv_sword_t sensor_ioctl (void *handler, void *op, void *p)
{
    int32_t listener_id = 0;
    switch ((int32_t)op) {
        case IOCTL_IRQ :
                tsc2046.tim_it_handle();
            break;
        case IOCTL_DMA :
            break;
        case SENSOR_WRITE :
            break;
        case SENSOR_ADD | SENSOR_CTL: 
                        listener_id = get_avail_listener();
                        if (listener_id >= 0) {
                            sensor_listeners[listener_id] = (TouchSensor *)handler;
                            sensor_listeners[listener_id]->clearEvent();
                             sensor_listeners[listener_id]->removeAllListeners();
                        }
            break;
        case SENSOR_REM | SENSOR_CTL:
                        listener_id = get_match_listener((TouchSensor *)handler);
                        if (listener_id >= 0) {
                            sensor_listeners[listener_id] = NULL;
                        }
            break;
        case SENSOR_INV | SENSOR_CTL:
                        if (tsc2046.hasEvent())
                            return join_sensor_task(3, NULL);
            break;
        case SENSOR_CAL | SENSOR_CTL:
                        return join_sensor_task(1, NULL);
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

static void tsc2046_evt_listener (abstract::Event e)
{
    for (int i = 0; i < SENSOR_MAX_LISTENERS; i++) {
        if (sensor_listeners[i] != NULL) {
            sensor_listeners[i]->setEvent((void *)e.getSource(), e.getCause());
        }
    }
}

static INT32_T sensor_task (WORD_T size, void *argv)
{
    vm::Mutex __frame(FRAME_RENDER_LOCK_ID);
    vm::Mutex __sensor(SENSOR_LOCK_ID);
    vm::Mutex __flash(FILE_SYSTEM_LOCK_ID);
    pair_t *pair = (pair_t *)argv;
    int tsc2046_cal_res = TSC2046_CAL_FILE_NOT_FOUND;
    TouchPointTypeDef tp;
    switch (size) {
        case 0:
            tsc2046.init(false);

            tsc2046_cal_res = tsc2046.loadCalData((char *)"tsc2046.cal");
            if (tsc2046_cal_res == TSC2046_CAL_FILE_NOT_FOUND) {
                tsc2046_cal_res = tsc2046.calibration((char *)"tsc2046.cal");
            }
            if (tsc2046_cal_res != TSC2046_CAL_OK) {
                return -1;
            }
            width = pair->a;
            height = pair->b;
            tsc2046.removeAllListeners();
            tsc2046.addListener(tsc2046_evt_listener);
            break;
        case 1:
            if (tsc2046.calibration((char *)"tsc2046.cal") != TSC2046_CAL_OK) {
                return -1;
            }
            break;
        case 3:
            tsc2046.invoke(&tp, width, height);
        break;

    }
    return 0;
}

static INT32_T join_sensor_task (WORD_T size, void *argv)
{
    THREAD_HANDLE th;
    th.Callback = (void *)sensor_task;
    th.Name = sensor_drv.name;
    th.Priority = 5;
    th.StackSize = VM_DEF_THREAD_HEAP_SIZE;
    th.argSize = size;
    th.Arg = argv;
    dispatch_from_svc(ret, VMAPI_CALL, (WORD_T)&th);
    return ret.ERROR;
}
