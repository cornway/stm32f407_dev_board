#ifndef VM_THREAD
#define VM_THREAD

#ifdef __cplusplus
    extern "C" {
#endif

#include "stdint.h"
#include "vm_conf.h"
#include "machM4.h"
#include "thread_types.h"

_VALUES_IN_REGS ARG_STRUCT_T VMBREAK (UINT_T ret);

void t_init_core (void);
void t_destroy (THREAD *t);
int t_init (THREAD **t, _CALLBACK callback, BYTE_T priority, WORD_T id, WORD_T heap_size, BYTE_T privilege, const char *name, WORD_T arg_size, void *arg);
THREAD *t_ready (UINT_T priority);
THREAD *t_search (const char *name);
void t_unlink_ready (THREAD *t);
void t_link_chain (THREAD *t, THREAD *l);
void t_unchain (void (*linker) (THREAD *), THREAD *t);
void t_link_ready (THREAD *t);
void t_link_drop (THREAD *t);
void t_link_delay (THREAD *t);
void t_link_fault (THREAD *t);
void t_link_notify (THREAD *t);
void t_unlink_notify (THREAD *t);
void t_link_mail (THREAD *t);
void t_unlink_mail(THREAD *t);
THREAD *t_mail (const char *name);
void t_link_cond (THREAD *t);
WORD_T t_check_cond (void (*linker) (THREAD *));
void t_link_wait_event (THREAD *t);
WORD_T t_fire_event (void (*linker) (THREAD *), const char *event_name);
THREAD *t_notify (const char *name);
int t_check_list (void);
void t_refresh (void (*linker) (THREAD *));
void t_tick (void (*linker) (THREAD *));
        
#ifdef __cplusplus
    }
#endif

#endif /*VM_THREAD*/


/*End of file*/

