#include "addresses.h"

int arrayOfWheelLocks[12][6] = {
        TRABARUEDAS_1,
        TRABARUEDAS_2,
        TRABARUEDAS_3,
        TRABARUEDAS_4,
        TRABARUEDAS_5,
        TRABARUEDAS_6,
        TRABARUEDAS_7,
        TRABARUEDAS_8,
        TRABARUEDAS_9,
        TRABARUEDAS_10,
        TRABARUEDAS_11,
        TRABARUEDAS_12
    };

void getBroadcastAddress(uint8_t* trabaruedas_address, uint8_t near_dev){
    // Define an array to hold the first 6 elements
    uint8_t firstSixElements[6];
    // Copy the first 6 elements of the first row into the new array
    for (int i = 0; i < 6; i++) {
        firstSixElements[i] = arrayOfWheelLocks[near_dev][i];
    }
    Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
                  firstSixElements[0], firstSixElements[1], firstSixElements[2],
                  firstSixElements[3], firstSixElements[4], firstSixElements[5]);
    memcpy(trabaruedas_address, firstSixElements, 6);
}

/*
void init_dev_structure(device_info structure[]){
    for(int i = 0; i<CANTIDAD_TRAB; i++){
        structure[i].ID = i;
        //strcpy(structure[i].addresses, TRABARUEDAS_1);
    }
}
*/