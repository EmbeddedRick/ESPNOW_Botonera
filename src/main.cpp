#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>
#include "ranging.h"
#include "driver/rtc_io.h"
#include "addresses.h"
// C libaries
#include <string.h>
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* strtol */

//// SWITCH ////
int buttonStatePrevious = HIGH;                      // previousstate of the switch
unsigned long minButtonLongPressDuration = 2000;    // Time we wait before we see the press as a long press
unsigned long buttonLongPressMillis;                // Time in ms when we the button was pressed
bool buttonStateLongPress = false;                  // True if it is a long press
const int intervalButton = 50;                      // Time between two readings of the button state
unsigned long previousButtonMillis;                 // Timestamp of the latest reading
unsigned long buttonPressDuration;                  // Time the button is pressed in ms
unsigned long previousLEDMillis;
const int intervalLED = 1000;
//// GENERAL ////
unsigned long currentMillis;          // Variable to store the number of milleseconds since the Arduino has started

// void readButtonState() prototype
void readButtonState();

#define peer_unconnected     2
#define peer_connected       5
#define mainButton           4 
bool doConnect = false;
bool connection_success = false;

//uint8_t broadcastAddress[] =   {0x34, 0x98, 0x7A, 0x73, 0xE6, 0x64};//{0x64, 0xE8, 0x33, 0x58, 0x12, 0x78};
uint8_t broadcastAddress[6];

typedef struct struct_message {
  char a[8];
} struct_message;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  // Desactiva el detector de brownout
  pinMode(mainButton, INPUT_PULLUP);
  pinMode(peer_connected, OUTPUT);
  pinMode(peer_unconnected, OUTPUT);
  digitalWrite(peer_unconnected, HIGH);
  digitalWrite(peer_connected, LOW);

  xTaskCreatePinnedToCore(
    uwbTask,   // Función de la tarea
    "TaskUWB", // Nombre de la tarea
    15000,     // Tamaño del stack de la tarea
    NULL,      // Parámetro de entrada (no usado aquí)
    2,         // Prioridad de la tarea
    NULL,      // Identificador de la tarea (no necesario aquí)
    1);        // Núcleo 1

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  //closest_dev = 1; // <- ESTO SIMULA EL DISPOSITIVO MÁS CERCANO
}

void loop() {
  currentMillis  = millis();
  readButtonState();
  if(doConnect){
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;  
    peerInfo.encrypt = false;
    
    // Add peer        
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
      Serial.println("Failed to add peer");
      doConnect = false;
      return;
    }
    Serial.println("Connection to peer Success!");
    digitalWrite(peer_connected, HIGH);
    digitalWrite(peer_unconnected, LOW);
    connection_success = true;
    doConnect = false;
    strcpy(myData.a, "Change");
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    if (result == ESP_OK) {
      Serial.println("Sent with success");
     }
     else {
       Serial.println("Error sending the data");
     }
  }else{
    if(closest_dev!=0 && !connection_success){
      if(currentMillis - previousLEDMillis > intervalLED) {
        digitalWrite(peer_unconnected, !digitalRead(peer_unconnected));
        Serial.print("Your closest device ID is: ");
        Serial.println(closest_dev);
        previousLEDMillis  = millis();
      }
    }else{
      if(!connection_success){
        digitalWrite(peer_unconnected, LOW);
      }
    }
  }
}

// Function for reading the button state
void readButtonState() {
    if(currentMillis - previousButtonMillis > intervalButton) {

      int buttonState = digitalRead(mainButton);    

      if (buttonState == LOW && buttonStatePrevious == HIGH && !buttonStateLongPress) {
        buttonLongPressMillis = currentMillis;
        buttonStatePrevious = LOW;
        Serial.println("Button pressed");
      }

      buttonPressDuration = currentMillis - buttonLongPressMillis;

      if (buttonState == LOW && !buttonStateLongPress && buttonPressDuration >= minButtonLongPressDuration) {
        buttonStateLongPress = true;
        Serial.println("Button long pressed");
        if(connection_success){
            strcpy(myData.a, "Release");
            esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
            if (result == ESP_OK) {
              Serial.println("Sent with success");
              connection_success = false;
              doConnect = false;
              closest_dev = 0;
              digitalWrite(peer_connected, LOW);
              digitalWrite(peer_unconnected, HIGH);
              delay(5000);
              esp_restart();
            }
            else {
              Serial.println("Error sending the data");
            }
          }
      }
        
      if (buttonState == HIGH && buttonStatePrevious == LOW) {
        buttonStatePrevious = HIGH;
        buttonStateLongPress = false;
        if (buttonPressDuration < minButtonLongPressDuration) {
          Serial.println("Button pressed shortly");
          if(closest_dev!=0 && !connection_success){
            getBroadcastAddress(broadcastAddress, closest_dev-1); // <- DESCOMENTAR
            doConnect = true;
          }else if(connection_success && wheelLock_tags[closest_dev].range > 4.0){ 
            strcpy(myData.a, "Change");
            esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
            if (result == ESP_OK) {
              Serial.println("Sent with success");
            }
            else {
              Serial.println("Error sending the data");
            }
          }
        }
        Serial.println("Button released");
      }
      previousButtonMillis = currentMillis;
    }
}