#ifndef GLOBAL_H
#define GLOBAL_H

#include <Arduino.h>
#include "EEPROM.h"


// Variables para envio y recepcion de mensajes
extern volatile boolean received;
extern String message;


extern float distance;
extern float distance_nearest;
extern int closest_dev;
extern const uint16_t Adelay;

extern bool connectReceived;
extern bool waitingForDistance;

enum State
{
    STATE_SLEEP,
    STATE_OPEN_1,
    STATE_CLOSED_1,
    STATE_OPEN_2,
    STATE_CLOSED_2
};


extern State currentState;
extern bool stateSleepSent;
extern const uint32_t botoneraId; 
extern const uint32_t panelId;

#endif 
 