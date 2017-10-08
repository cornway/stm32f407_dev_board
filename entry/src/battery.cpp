#include <stdint.h>
#include "vm.h"
#include "app_def.h"
#include "device_conf.h"
#include "gui.h"
#include <stdio.h>

static char batteryValueStr[8];
void drawBattery (NonPalette<color_t, range_t, COLOR_WHITE> *component)
{
    batteryData *data = (batteryData *)component->getUserData();
    float batteryLevel = ((2.5f * (float)*data->batteryValue) * 2.0f / 4096.0f) / 3.3f;
    
    //component->getGraphic()->drawString(component->getBox(), "BATTERY :", 0x0);
    range_t w = component->w - 4;
    range_t h = component->h;
    range_t batFull = (range_t)((float)w * batteryLevel);

    Box<range_t> bfull = {component->x, component->y, batFull, h};
    Box<range_t> bempty = {component->x + batFull, component->y, w - batFull, h};
    Box<range_t> dmetal = {component->x + w, component->y + h / 2 - h / 4, 4, h / 2};

    auto g = component->getGraphic();
    
    g->fill(bfull, *data->batteryFullColor | component->getGlow());
    g->fill(bempty, *data->batteryEmptycolor | component->getGlow());
    g->fill(dmetal, *data->batteryMetalColor | component->getGlow());
    
    sprintf(batteryValueStr, "  %d%%", (int)(100.0f * batteryLevel));
    //d(component->getTextBox(batteryValueStr, g->getFontWidth(), g->getFontHeight()));
    //d.setOrigins(component->getOrigins());
    component->getGraphic()->drawString(component->getBox(), batteryValueStr, *data->batteryTextColor);
}