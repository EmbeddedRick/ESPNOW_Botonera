#ifndef RANGING_H
#define RANGING_H
#include "DW1000Ranging.h"
#include "DW1000.h"
#include "global.h"
#include "FreeRTOS.h"
#include "freertos/task.h"

//------------------UWB------------------------
#define SPI_SCK             12
#define SPI_MISO            13
#define SPI_MOSI            11
#define DW_CS               10
#define PIN_RST             8
#define PIN_IRQ             6
#define PIN_SS              10


// leftmost two bytes below will become the "short address"
// Dirección MAC del anchor
#define TAG_ADD  "FF:FF:FF:FF:FF:FF:FF:FF"

typedef struct ble_dev_range{
  uint8_t connection;
  uint8_t id;
  float range;
}device_info;

extern device_info wheelLock_tags[10];

// prototypes
void newRange();
void newDevice(DW1000Device *device);
void inactiveDevice(DW1000Device *device);
int near_device(device_info dev_info[]);
void init_UWB();
void stop_UWB();
void uwb_main(uint8_t state);

void uwbTask(void *pvParameters);

#endif

