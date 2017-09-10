#ifndef VMAPI_CALL_H
#define VMAPI_CALL_H

#ifdef __cplusplus
    extern "C" {
#endif
        
#define DEPREC_IDLE_CALL    (0x80U)

#define NEXT(a) (a + 1)
        
enum {
    VMAPI_RESTART       = NEXT(-1),
    VMAPI_SLEEP         = DEPREC_IDLE_CALL | NEXT(VMAPI_RESTART),
    VMAPI_YIELD         = NEXT(VMAPI_SLEEP),
    VMAPI_CREATE        = NEXT(VMAPI_YIELD),
    VMAPI_LOCK          = DEPREC_IDLE_CALL | NEXT(VMAPI_CREATE),
    VMAPI_UNLOCK        = DEPREC_IDLE_CALL | NEXT(VMAPI_LOCK),
    VMAPI_NOTIFY        = NEXT(VMAPI_UNLOCK),
    VMAPI_WAIT_NOTIFY   = DEPREC_IDLE_CALL | NEXT(VMAPI_NOTIFY),
    VMAPI_NOTIFY_WAIT   = DEPREC_IDLE_CALL | NEXT(VMAPI_WAIT_NOTIFY),
    VMAPI_SYNC          = DEPREC_IDLE_CALL | NEXT(VMAPI_NOTIFY_WAIT),
    VMAPI_WAIT          = DEPREC_IDLE_CALL | NEXT(VMAPI_SYNC),
    VMAPI_WAIT_EVENT    = DEPREC_IDLE_CALL | NEXT(VMAPI_WAIT),
    VMAPI_FIRE_EVENT    = NEXT(VMAPI_WAIT_EVENT),
    VMAPI_MAIL          = NEXT(VMAPI_FIRE_EVENT),
    VMAPI_WAIT_MAIL     = DEPREC_IDLE_CALL | NEXT(VMAPI_MAIL),
    VMAPI_TIMER_CREATE  = NEXT(VMAPI_WAIT_MAIL),
    VMAPI_TIMER_REMOVE  = NEXT(VMAPI_TIMER_CREATE),
    VMAPI_CALL          = NEXT(VMAPI_TIMER_REMOVE),
    VMAPI_CRITICAL      = NEXT(VMAPI_CALL),
    VMAPI_END_CRITICAL  = NEXT(VMAPI_CRITICAL),
    VMAPI_DRV_ATTACH    = NEXT(VMAPI_END_CRITICAL),
    VMAPI_DRV_DETTACH   = NEXT(VMAPI_DRV_ATTACH),
    VMAPI_DRV_IO        = NEXT(VMAPI_DRV_DETTACH),
    VMAPI_DRV_CTL       = NEXT(VMAPI_DRV_IO),
    
    VMAPI_EXIT          = NEXT(VMAPI_DRV_CTL),
    VMAPI_FAULT         = NEXT(VMAPI_EXIT),
    
    VMAPI_RESET         = 0x80000U,
};    
        
        
#ifdef __cplusplus
    }
#endif
        
#endif /*VMAPI_CALL_H*/