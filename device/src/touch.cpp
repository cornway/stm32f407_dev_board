#include "touch.h"

TouchSensor::TouchSensor ()
{
    this->point.x = 0;
    this->point.y = 0;
    this->point.hits = 0;
    this->point.onPressTime = 0;
    this->hasInvoke = false;
    this->event = SENSOR_ANY;
}

TouchSensor::~TouchSensor ()
{
    
}

void TouchSensor::setEvent (void *point, uint32_t event)
{
    this->point = *(TouchPointTypeDef *)point;
    this->event = event;
    this->hasInvoke = true;
}

void TouchSensor::invokeEvent ()
{
    if (this->hasInvoke == false) {
        return;
    }
    this->hasInvoke = false;
    this->tpoint = this->point;
    this->eventBurner.fireEvents( abstract::Event(&this->tpoint, this->event) );
    this->event = SENSOR_ANY;
}

void TouchSensor::clearEvent ()
{
    this->point.x = 0;
    this->point.y = 0;
    this->point.hits = 0;
    this->point.onPressTime = 0;
    this->hasInvoke = false;
    this->event = SENSOR_ANY;
}

void TouchSensor::setId (uint32_t id)
{
    this->id = id;
}

uint32_t TouchSensor::getId ()
{
    return this->id;
}

