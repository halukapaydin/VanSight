#include "ui_event_handlers.h"
#include <stdio.h>
#include "ui/src/ui.h"
#include "espnow_stub.h"

// SquareLine Studio Event Handler Implementations
// Bu dosya SquareLine Studio tarafından yönetilmez, güvenle düzenleyebilirsiniz

// Relay mapping (same as ESP32)
#define BTN_HOME 0
#define BTN_LEFT_CHAIR 1
#define BTN_RIGHT_CHAIR 2
#define BTN_KITCHEN 3
#define BTN_TOILET 4
#define BTN_BATH 5
#define BTN_BED 6
#define BTN_BALKONY_UP 7
#define BTN_BALKONY_DOWN 8
#define BTN_PROJECTOR 9

const uint8_t BUTTON_TO_RELAY_MAP[] = {
    1,   // BTN_HOME
    2,   // BTN_LEFT_CHAIR
    3,   // BTN_RIGHT_CHAIR
    4,   // BTN_KITCHEN
    5,   // BTN_TOILET
    6,   // BTN_BATH
    7,   // BTN_BED
    8,   // BTN_BALKONY_UP
    9,   // BTN_BALKONY_DOWN
    10   // BTN_PROJECTOR
};

void onBtnHomeClick(lv_event_t * e)
{
    printf("Home button clicked!\n");
    fflush(stdout);
    
    // Send relay toggle via ESP-NOW stub
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "{\"cmd\":\"relay_toggle\",\"relay\":%d}", BUTTON_TO_RELAY_MAP[BTN_HOME]);
    esp_now_send(NULL, (const uint8_t*)cmd, strlen(cmd));
}

void onBtnLeftChairClick(lv_event_t * e)
{
    printf("Left Chair button clicked!\n");
    fflush(stdout);
    
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "{\"cmd\":\"relay_toggle\",\"relay\":%d}", BUTTON_TO_RELAY_MAP[BTN_LEFT_CHAIR]);
    esp_now_send(NULL, (const uint8_t*)cmd, strlen(cmd));
}

void onBtnRightChairClick(lv_event_t * e)
{
    printf("Right Chair button clicked!\n");
    fflush(stdout);
    
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "{\"cmd\":\"relay_toggle\",\"relay\":%d}", BUTTON_TO_RELAY_MAP[BTN_RIGHT_CHAIR]);
    esp_now_send(NULL, (const uint8_t*)cmd, strlen(cmd));
}

void onBtnKitchenClick(lv_event_t * e)
{
    printf("Kitchen button clicked!\n");
    fflush(stdout);
    
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "{\"cmd\":\"relay_toggle\",\"relay\":%d}", BUTTON_TO_RELAY_MAP[BTN_KITCHEN]);
    esp_now_send(NULL, (const uint8_t*)cmd, strlen(cmd));
}

void onBtnToiletClick(lv_event_t * e)
{
    printf("Toilet button clicked!\n");
    fflush(stdout);
    
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "{\"cmd\":\"relay_toggle\",\"relay\":%d}", BUTTON_TO_RELAY_MAP[BTN_TOILET]);
    esp_now_send(NULL, (const uint8_t*)cmd, strlen(cmd));
}

void onBtnBathClick(lv_event_t * e)
{
    printf("Bath button clicked!\n");
    fflush(stdout);
    
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "{\"cmd\":\"relay_toggle\",\"relay\":%d}", BUTTON_TO_RELAY_MAP[BTN_BATH]);
    esp_now_send(NULL, (const uint8_t*)cmd, strlen(cmd));
}

void onBtnBedClick(lv_event_t * e)
{
    printf("Bed button clicked!\n");
    fflush(stdout);
    
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "{\"cmd\":\"relay_toggle\",\"relay\":%d}", BUTTON_TO_RELAY_MAP[BTN_BED]);
    esp_now_send(NULL, (const uint8_t*)cmd, strlen(cmd));
}

void onBtnBalkonyUpClick(lv_event_t * e)
{
    printf("Balkony Up button clicked!\n");
    fflush(stdout);
    
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "{\"cmd\":\"relay_toggle\",\"relay\":%d}", BUTTON_TO_RELAY_MAP[BTN_BALKONY_UP]);
    esp_now_send(NULL, (const uint8_t*)cmd, strlen(cmd));
}

void onBtnBalkonyDownClick(lv_event_t * e)
{
    printf("Balkony Down button clicked!\n");
    fflush(stdout);
    
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "{\"cmd\":\"relay_toggle\",\"relay\":%d}", BUTTON_TO_RELAY_MAP[BTN_BALKONY_DOWN]);
    esp_now_send(NULL, (const uint8_t*)cmd, strlen(cmd));
}

void onBtnProjectorClick(lv_event_t * e)
{
    printf("Projector button clicked!\n");
    fflush(stdout);
    
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "{\"cmd\":\"relay_toggle\",\"relay\":%d}", BUTTON_TO_RELAY_MAP[BTN_PROJECTOR]);
    esp_now_send(NULL, (const uint8_t*)cmd, strlen(cmd));
}

void onBtnReloadInformationClick(lv_event_t * e)
{
    printf("Reload Information button clicked!\n");
    fflush(stdout);
    
    const char* cmd = "{\"cmd\":\"all_status\"}";
    esp_now_send(NULL, (const uint8_t*)cmd, strlen(cmd));
}

void onBtnCloseAllClick(lv_event_t * e)
{
    printf("Close All button clicked!\n");
    fflush(stdout);
    
    const char* cmd = "{\"cmd\":\"all_relays_off\"}";
    esp_now_send(NULL, (const uint8_t*)cmd, strlen(cmd));
}
