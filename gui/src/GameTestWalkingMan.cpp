#include "GameTestWalkingMan.h"

extern tAnimation<uint16_t> throwingSkin;

int DefaultWalkingManClickListener (TouchStruct arg)
{
    NonPaletteInterface<ColorDepth> *I = (NonPaletteInterface<ColorDepth> *)arg.object;
    WalkingManObject<ColorDepth> *man = (WalkingManObject<ColorDepth> *)I->getUserObject();
        
        int xS = arg.local.x - man->x;
        int yS = arg.local.y - man->y;
        if (xS < 0) {
            man->direction = 2;
        } else {
            man->direction = 0;   
        }
    if (arg.clickCount >= 2) { 
        man->throwWeapon(10, man->direction);
    } else {
        man->xTarget = arg.local.x;
        man->yTarget = arg.local.y;
        man->action = WalkingManWalk;
        int dir = 0;
        if ((xS > 0 ? xS : -xS) > (yS > 0 ? yS : -yS)) {
            man->stepX = xS > 0 ? 5 : -5;
            man->neededStepCount = xS / man->stepX;
            man->stepY = yS / man->neededStepCount;
        } else {
            man->stepY = yS > 0 ? 5 : -5;
            man->neededStepCount = yS / man->stepY;
            man->stepX = xS / man->neededStepCount; 
        }
    }
    return -1;
}
int DefaultWalkingManTouchListener (TouchStruct arg)
{
    NonPaletteInterface<ColorDepth> *I = (NonPaletteInterface<ColorDepth> *)arg.object;
    WalkingManObject<ColorDepth> *man = (WalkingManObject<ColorDepth> *)I->getUserObject();
    
    return -1;
}
int DefaultWalkingManReleaseListener (TouchStruct arg)
{
    NonPaletteInterface<ColorDepth> *I = (NonPaletteInterface<ColorDepth> *)arg.object;
    WalkingManObject<ColorDepth> *man = (WalkingManObject<ColorDepth> *)I->getUserObject();
    
    return -1;
}


int WalkingManRepaintHook (void *parent)
{
    NonPaletteInterface<ColorDepth> *I = (NonPaletteInterface<ColorDepth> *)parent;
    WalkingManObject<ColorDepth> *man = (WalkingManObject<ColorDepth> *)I->getUserObject();
    if (man->action > 0) {
        switch (man->action) {
            case WalkingManWalk:
                break;
            case WalkingManThrow:
                man->graphic->fill(man->getBox(), man->throwingSkin.getDiscont(), 0, man->direction);
                break;
            
            default :
                break;
        }
    } else if (man->neededStepCount > 0) {
        man->x += man->stepX;
        man->y += man->stepY;
        man->neededStepCount--;
        man->graphic->fill(man->getBox(), man->walkingSkin.getDiscont(), 0, man->direction);
    } else {
        man->graphic->fill(man->getBox(), man->walkingSkin.getDiscont(1), 0, man->direction);
    }
    if (man->throwingWeapon.isTimeElapse() == false) {
                man->throwingWeapon.x += man->throwingWeapon.dx;
    }
    return 0;
}



void WalkingManThrowingWeaponRepaintHook (void *arg)
{
    static int frameCount = 0;
    
    GraphicObject<ColorDepth> *o = (GraphicObject<ColorDepth> *)arg;
    static GAnimation<ColorDepth> animation( throwingSkin );
    o->getGraphic()->fill(o->getBox(), animation.getDiscont() , 0, 0);
}


int DefaultWalkAnimationHook (void *a, void *p)
{
    
    return 0;
}
int DefaultThrowAnimationHook (void *a, void *p)
{
    WalkingManObject<ColorDepth> *man = (WalkingManObject<ColorDepth> *) p;
    man->action = WalkingManWalk;
    return 0;
}

