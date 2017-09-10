#ifndef VM_DRV_H
#define VM_DRV_H

#include "vm_conf.h"
#include <stdint.h>
#include "iterable.h"

enum {
    IOCTL_START = 0,
    IOCTL_IRQ,
    IOCTL_DMA,
    IOCTL_END,
};

typedef int32_t drv_sword_t;
typedef uint32_t drv_word_t;
typedef uint16_t drv_hword_t;
typedef uint8_t drv_byte_t; 
typedef uint8_t* drv_p_t;

typedef struct {
    drv_p_t dest;
    drv_p_t source;
    drv_hword_t control;
    drv_hword_t length;
} drv_data_t;    

typedef struct drv_handle_t {
    drv_sword_t (*load) (uint32_t, uint32_t);
    drv_sword_t (*unload) (uint32_t);
    drv_sword_t (*probe) (uint32_t);
    drv_sword_t (*ioctl) (void *, uint32_t, void *);
    drv_sword_t (*io) (void *, uint32_t, void *);
};

typedef struct {
    uint32_t id;
    int32_t irq;
    int32_t dma[3];
    uint32_t ctl;
    uint32_t size;
    drv_handle_t handle;
    char name[1];
} drv_t;

drv_t *drv_get (int32_t id);
int32_t drv_attach (drv_handle_t *handle, char *name, int32_t irq, int32_t dma);
int32_t drv_detach (uint32_t id);
int32_t drv_detach (char *name);
int32_t drv_irq (int32_t id);
int32_t drv_dma (int32_t id);

#endif /*VM_DRV_H*/