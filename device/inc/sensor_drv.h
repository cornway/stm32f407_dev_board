#ifndef SENSOR_DRV_H
#define SENSOR_DRV_H

#include "vmapi.h"

#ifdef NEXT
#undef NEXT
#endif
#define NEXT(x) ((x) << 1)

enum {
    SENSOR_READ = 1,
    SENSOR_WRITE = NEXT(SENSOR_READ),
    SENSOR_CTL = NEXT(SENSOR_WRITE),
    SENSOR_ADD = NEXT(SENSOR_CTL),
    SENSOR_REM = NEXT(SENSOR_ADD),
    SENSOR_INV = NEXT(SENSOR_REM),
    SENSOR_CAL = NEXT(SENSOR_INV),
};

_EXTERN drv_handle_t sensor_drv;

#endif /*SENSOR_DRV_H*/
