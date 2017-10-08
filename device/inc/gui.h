#ifndef GUI_H__
#define GUI_H__

#include "guiEngine.h"
#include "abstract.h"
//#include "font_16bpp.h"
#include "font_array.h"
#include "device_conf.h"
#include "device_gui_conf.h"



class DevGui {
	private :

	public :
        DevGui()
        {
            
        }
        ~DevGui()
        {
            
        }

        static void fill_vpage(int page, void *color)
        {
            int d = TFT_WIDTH * TFT_HEIGHT + 1;
            color_t *buf = (color_t *)FRAME_MEMORY_BASE;
            for (int i = 0; i < d; i++) {
                buf[i] = *(color_t *)color;
            }
        }
};


#endif /*GUI_H__*/

