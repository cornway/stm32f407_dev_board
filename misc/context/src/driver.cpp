#include "driver.h"
#include "mach.h"
#include <string.h>
#include <errno.h>

extern "C" void *vmalloc (UINT_T size);
extern "C" void vmfree (void *p);

static drv_t *drivers[VM_MAX_DRIVERS];

#define ASZ(a) (sizeof(a) / sizeof(a[0]))
    
static int32_t drv_get_first ();
static int32_t drv_init (int32_t irq, int32_t dma);

drv_t *drv_get (int32_t id)
{
    for (uint8_t i = 0; i < VM_MAX_DRIVERS; i++) {
        if (drivers[i] != NULL) {
            if(drivers[i]->id == id) {
                return drivers[i];
            }
        }
    }
    return NULL;
}

int32_t  drv_attach (drv_handle_t *handle, int32_t irq, int32_t dma)
{
    int32_t id = drv_init(irq, dma);
    if (id < 0) {
        return -ENOMEM;
    }
    drivers[id]->handle = *handle;
    if (drivers[id]->handle.probe == NULL || 
        drivers[id]->handle.load == NULL ) {
        drv_detach(id);
        return -ENOMEM;
    }
    if (drivers[id]->handle.load(0, 0) < 0) {
        drv_detach(id);
        return -ENOMEM;
    }
    if (drivers[id]->handle.probe(0) < 0) {
        drv_detach(id);
        return -ENOMEM;
    }
    return id;
}

int32_t drv_detach (uint32_t id)
{
    if (drivers[id]->id != id) {
        return -ERANGE;
    }
    if (drivers[id]->handle.unload == NULL) {
        return -ENOMEM;
    }
    drivers[id]->handle.unload(0);
    memset(drivers[id], 0, drivers[id]->size);
    vmfree(drivers[id]);
    drivers[id] = NULL;
    return 0;
}

int32_t drv_detach (const char *name)
{
    for (uint8_t i = 0; i < VM_MAX_DRIVERS; i++) {
        if (drivers[i] != NULL) {
            if(strcmp(name, drivers[i]->handle.name) == 0) {
                return drv_detach(drivers[i]->id);
            }
        }
    }
    return -ERANGE;
}

int32_t drv_irq (int32_t id)
{
    int32_t error;
    for (uint8_t i = 0; i < VM_MAX_DRIVERS; i++) {
        if(drivers[i] != NULL) {
            if (drivers[i]->handle.ioctl == NULL) {
                /*TODO error*/
            } else if (drivers[i]->irq == id) {
                error |= drivers[i]->handle.ioctl(drivers[i], (void *)IOCTL_IRQ, drivers[i]);
            }
        }
    }
    return 0;
}

int32_t drv_dma (int32_t id)
{
    int32_t error;
    for (uint8_t i = 0; i < VM_MAX_DRIVERS; i++) {
        if(drivers[i] != NULL) {
            if (drivers[i]->dma[0] == id) {
                error |= drivers[i]->handle.ioctl(drivers[i], (void *)IOCTL_DMA, drivers[i]);
            } 
        }
    }
    return 0;
}

static int32_t drv_get_first ()
{
    for (uint8_t i = 0; i < VM_MAX_DRIVERS; i++) {
        if (drivers[i] == NULL) {
            return i;
        }
    }
    return -ENOMEM;
}


static int32_t drv_init (int32_t irq, int32_t dma)
{
    int32_t id = 0;
    if (id >= VM_MAX_DRIVERS) {
        return -ENOMEM;
    }
    id = drv_get_first();
    if (id < 0) {
        return id;
    }
    int size = sizeof(drv_t) + 2;
    drv_t *drv = (drv_t *)vmalloc(size);
    if (drv == NULL) {
        return -ENOMEM;
    }
    drivers[id] = drv;
    drv->id = id;
    drv->size = size;
    drv->irq = irq;
    drv->dma[0] = dma;
    drv->dma[1] = 0;
    drv->dma[2] = 0;
    return id;
}

int32_t drv_get_id (const char *name)
{
    for (uint8_t i = 0; i < VM_MAX_DRIVERS; i++) {
        if(drivers[i] != NULL) {
            if (!strcmp(drivers[i]->handle.name, name)) {
                return i;
            } 
        }
    }
    return -ERANGE;
}