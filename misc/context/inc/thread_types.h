#ifndef THREAD_TYPES_H
#define THREAD_TYPES_H

#include <stdint.h>
#include "machM4.h"

#ifndef     THREAD_MAX_COUNT    
#define     THREAD_MAX_COUNT (24U)
#endif

#ifndef     VM_THREAD_MAX_PRIORITY  
#define     VM_THREAD_MAX_PRIORITY  (8U)
#endif
        
#ifndef     VM_DEF_THREAD_HEAP_SIZE
#define     VM_DEF_THREAD_HEAP_SIZE (6144U)
#endif
        
#ifndef     VM_MIN_THREAD_HEAP_SIZE
#define     VM_MIN_THREAD_HEAP_SIZE (FPU_STACK_SIZE + CPU_STACK_SIZE + 128)
#endif
        
#ifndef     VM_DEF_THREAD_NAME_LEN
#define     VM_DEF_THREAD_NAME_LEN (24U)
#endif
        
#ifndef     NULL
#define     NULL    ((void *)0)
#endif

#define     IDLE_THREAD_ID  (0U)    

typedef _PACKED struct {
    void *Callback;
    WORD_T StackSize;
    BYTE_T Priority;
    const char *Name;
    WORD_T argSize;
    void *Arg;
} THREAD_HANDLE;
       
typedef INT32_T (*THREAD_COND_T) (WORD_T type, void *link);
        
enum {
    THREAD_STOP = 0x0U,
    THREAD_RUN  = 0x1U,
    THREAD_PEND = 0x2U,
};

enum {
    THREAD_NO_FPU = 0x0U,
    THREAD_FPU = 0x1U,
};

enum {
    T_OK,
    T_LONG_NAME,
    T_SMALL_HEAP,
    T_SMALL_CORE,
    T_NULL_CALLBACK,
    T_PRIV_UNDEF,
    T_PRIOR_UNDEF,
};


        
typedef _PACKED struct {
    void *firstLink;
    void *lastLink;
    BYTE_T elements;
} THREAD_LIST;

typedef _PACKED struct {
    INT64_T cpuUsage;
    CPU_STACK_FRAME *CPU_FRAME;
    WORD_T DELAY;
    WORD_T STACK;
    WORD_T STACK_SIZE;
    INT32_T  ID;
    void *nextLink;
    void *prevLink;
    const char *faultMessage;
    _PACKED union {
        void *link;
        THREAD_COND_T cond;
        const char *event;
    };
    void *caller;
    THREAD_LIST chain;
    BYTE_T PRIORITY;
    BYTE_T V_PRIORITY;
    BYTE_T PRIVILEGE;
    char name[VM_DEF_THREAD_NAME_LEN];
    unsigned STATUS : 3;
    unsigned USE_FPU : 1;
    unsigned mutex : 1;
    unsigned monitor : 1;
    unsigned fault : 1;
    unsigned waitNotify : 1;
} THREAD;


#endif /*THREAD_TYPES_H*/
