#include "global.h"

// Inicialización de variables
float distance = 0;
float distance_nearest = 0;
int closest_dev = 0;
const uint16_t Adelay = 16580;

bool connectReceived = false;
bool waitingForDistance = false;

State currentState = STATE_SLEEP;
bool stateSleepSent = false;


const uint32_t botoneraId  = 861409900; // Se agrega el id de la botonera
const uint32_t panelId = 503061709;  // se agrega el id del panel
