#include "render_app.h"
#include "fgpu.h"
#include "gui.h"
#include "TFTili9486.h"



TFT_ILI9486 tft;

INT32_T render_app (WORD_T size, void *argv)
{
    volatile uint32_t i = 0;
    //tft.init();
    FG_Rect fgRect;
    fgRect.x = 0;
    fgRect.y = 0;
    fgRect.w = TFT_WIDTH;
    fgRect.h = TFT_HEIGHT;
    DevGui::fill_vpage(0, 0);
    for (;;) {  
        if (applicationControl.tftControl.backlight == TFT_BACKLIGHT_OFF) {
            tft.setBackLight(0);
            vm::wait_event("wakeup");
        } else {
            tft.setBackLight(applicationControl.tftControl.backlightValue);
            vm::lock(FRAME_RENDER_LOCK_ID);
            gpu_wait();
            gpu_update(fgRect);
            vm::sleep(2);
            gpu_wait();
            vm::unlock(FRAME_RENDER_LOCK_ID);
        }
        vm::yield();
    }
    //return 0;
}

