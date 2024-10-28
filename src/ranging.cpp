#include "ranging.h"
device_info wheelLock_tags[10];

int near_device(device_info dev_info[]){
    uint8_t min = 20;
    uint8_t closest = 0;
    for(int i = 0; i<10; i++){
        if(dev_info[i].connection == 255 && dev_info[i].range < 6.0){
            if(dev_info[i].range < min){
                min = dev_info[i].range;
                closest = i;
            }
        }
    }
    return closest;
}

void newRange()
{
    uint8_t *short_add = DW1000Ranging.getDistantDevice()->getByteShortAddress();
    distance = DW1000Ranging.getDistantDevice()->getRange();
    //Serial.printf("Distancia medida: %.2f metros\n", distance);
    wheelLock_tags[short_add[0]].id = short_add[0];
    wheelLock_tags[short_add[0]].range = distance;
    /*
    Serial.printf("%02x:%02x\n",
                  short_add[0], short_add[1]);
    Serial.println(short_add[0], HEX); //64
    Serial.print("from: ");
    Serial.println(short_add[1], HEX); //E8
    */
    Serial.printf("\t Range: %f\n", wheelLock_tags[short_add[0]].range);
}

void newDevice(DW1000Device *device)
{
    //Serial.print("ranging init; 1 device added ! -> ");
    //Serial.print(" short:");
    Serial.println(device->getShortAddress(), DEC);
    wheelLock_tags[device->getShortAddress()].connection = 255;
}

void inactiveDevice(DW1000Device *device)
{
    //Serial.print("delete inactive device: ");
    Serial.println(device->getShortAddress(), HEX);
    wheelLock_tags[device->getShortAddress()].connection = 250;
}

void init_UWB()
{
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ);
    DW1000Ranging.attachNewRange(newRange);
    DW1000Ranging.attachNewDevice(newDevice);
    DW1000Ranging.attachInactiveDevice(inactiveDevice);
    DW1000Ranging.startAsTag(TAG_ADD, DW1000.MODE_LONGDATA_RANGE_LOWPOWER,false);
}



void stop_UWB()
{
    detachInterrupt(digitalPinToInterrupt(PIN_IRQ));
    SPI.end();
    Serial.println("UWB desconectado.");
}


void uwbTask(void *pvParameters)
{
    init_UWB();
    while (true)
    {
        DW1000Ranging.loop();
        closest_dev = near_device(wheelLock_tags);
        vTaskDelay(1 / portTICK_PERIOD_MS); // Yield to other tasks
    }
}



