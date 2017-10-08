
#include "vmapi.h"

#define upc_(...) \
do { \
    ARG_STRUCT_T arg = {__VA_ARGS__}; \
    return upc(arg); \
} while (0)


_VALUES_IN_REGS ARG_STRUCT_T vm::restart ()
{
    upc_();
}

_VALUES_IN_REGS ARG_STRUCT_T vm::sleep (UINT32_T delay)
{
    upc_(VMAPI_SLEEP, delay);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::yield (void)
{
    upc_(VMAPI_YIELD);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::create (THREAD_HANDLE *th)
{
    upc_(VMAPI_CREATE, (WORD_T)th);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::create (_CALLBACK callback, const char *name, WORD_T stack, WORD_T prio, WORD_T size, void *arg)
{
    THREAD_HANDLE th;
    th.Callback = (void *)callback;
    th.Name = name;
    th.Priority = prio;
    th.StackSize = stack;
    th.argSize = size;
    th.Arg = arg;
    return vm::create(&th);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::call (_CALLBACK callback, const char *name, WORD_T stack, WORD_T prio, WORD_T size, void *arg)
{
    THREAD_HANDLE th;
    th.Callback = (void *)callback;
    th.Name = name;
    th.Priority = prio;
    th.StackSize = stack;
    th.argSize = size;
    th.Arg = arg;
    upc_(VMAPI_CALL, (WORD_T)&th);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::drv_link (drv_handle_t *handler, uint32_t irq, uint32_t dma)
{
    upc_(VMAPI_DRV_ATTACH, (WORD_T)handler, irq, dma);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::drv_unlink (uint32_t id)
{
    upc_(VMAPI_DRV_DETTACH, id);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::drv_ctl (uint32_t id, uint32_t ctl0, uint32_t ctl1)
{
    upc_(VMAPI_DRV_CTL, id, ctl0, ctl1);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::drv_io (uint32_t id, drv_data_t *data)
{
    upc_(VMAPI_DRV_IO, id, (WORD_T)data);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::drv_probe (const char *name)
{
    upc_(VMAPI_DRV_PROBE, (WORD_T)name);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::lock (UINT32_T id)
{
    upc_(VMAPI_LOCK, id);
}    
_VALUES_IN_REGS ARG_STRUCT_T vm::unlock (UINT32_T id)
{
    upc_(VMAPI_UNLOCK, id);
}  

_VALUES_IN_REGS ARG_STRUCT_T vm::notify (const char *name)
{
    upc_(VMAPI_NOTIFY, (WORD_T)name);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::wait_notify ()
{
    upc_(VMAPI_WAIT_NOTIFY, 0);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::notify_wait (const char *name)
{
    upc_(VMAPI_NOTIFY_WAIT, (WORD_T)name);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::sync (const char *name)
{
    upc_(VMAPI_SYNC, (WORD_T)name);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::wait (THREAD_COND_T cond)
{
    upc_(VMAPI_WAIT, (WORD_T)cond);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::wait_event (const char *event_name)
{
    upc_(VMAPI_WAIT_EVENT, (WORD_T)event_name);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::fire_event (const char *event_name)
{
    upc_(VMAPI_FIRE_EVENT, (WORD_T)event_name);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::mail (char *name, MAIL_HANDLE *mail)
{
    upc_(VMAPI_MAIL, (WORD_T)name);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::wait_mail ()
{
    upc_(VMAPI_WAIT_MAIL);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::timer (WORD_T *dest, WORD_T id)
{
    upc_(VMAPI_TIMER_CREATE, WORD_T(dest), id);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::timer_remove (WORD_T id)
{
    upc_(VMAPI_TIMER_REMOVE, id);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::critical ()
{
    upc_(VMAPI_CRITICAL);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::end_critical ()
{
    upc_(VMAPI_END_CRITICAL);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::exit (UINT32_T ret)
{
    upc_(VMAPI_EXIT, ret);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::fault (const char *cause)
{
    upc_(VMAPI_FAULT, (WORD_T)cause);
}

_VALUES_IN_REGS ARG_STRUCT_T vm::reset ()
{
    upc_(VMAPI_RESET);
}


INT32_T VMAPI_ErrorHandler (WORD_T from, _VALUES_IN_REGS ARG_STRUCT_T arg)
{
    _UNUSED(from);
    _UNUSED(arg);
    for (;;) {}
    //return 0;
}

/*End of file*/

