#ifndef __DEVICE_H__
#define __DEVICE_H__
#include "stdint.h"

class Device {
    private :
        
    public :
        
        Device ();
        void init ();
        void run ();  
}; /*class Device*/


/*
TODO:
remove from here
*/
extern uint32_t load_program (void *memory, const char *path, const char *name);
extern uint32_t join_program (void *mem, const char *name, int c, char **v);
extern void *program_space_begin;

#endif /*__DEVICE_H__*/


