#include "ui_event_handlers.h"
#include <Arduino.h>
#include <VanSightLib.h>
#include <ui.h>

using namespace VanSight;

// Relay button mapping (button index to relay number)
const uint8_t RELAY_MAP[] = {
    1,  // Home
    2,  // Left Chair
    3,  // Right Chair
    4,  // Kitchen
    5,  // Toilet
    6,  // Bath
    7,  // Bed
    8,  // Balkony Up
    9,  // Balkony Down
    10  // Projector
};

// Debounce mechanism to prevent double events
static unsigned long lastClickTime[10] = {0};
const unsigned long DEBOUNCE_DELAY = 300; // 300ms debounce

void onBtnHomeClick(lv_event_t * e)
{
    unsigned long now = millis();
    if (now - lastClickTime[0] < DEBOUNCE_DELAY) {
        return; // Ignore duplicate event
    }
    lastClickTime[0] = now;
    
    Serial.println("Home button clicked!");
    BleCommandManager::getInstance().toggleRelay(RELAY_MAP[0]);
}

void onBtnLeftChairClick(lv_event_t * e)
{
    unsigned long now = millis();
    if (now - lastClickTime[1] < DEBOUNCE_DELAY) return;
    lastClickTime[1] = now;
    
    Serial.println("Left Chair button clicked!");
    BleCommandManager::getInstance().toggleRelay(RELAY_MAP[1]);
}

void onBtnRightChairClick(lv_event_t * e)
{
    unsigned long now = millis();
    if (now - lastClickTime[2] < DEBOUNCE_DELAY) return;
    lastClickTime[2] = now;
    
    Serial.println("Right Chair button clicked!");
    BleCommandManager::getInstance().toggleRelay(RELAY_MAP[2]);
}

void onBtnKitchenClick(lv_event_t * e)
{
    unsigned long now = millis();
    if (now - lastClickTime[3] < DEBOUNCE_DELAY) return;
    lastClickTime[3] = now;
    
    Serial.println("Kitchen button clicked!");
    BleCommandManager::getInstance().toggleRelay(RELAY_MAP[3]);
}

void onBtnToiletClick(lv_event_t * e)
{
    unsigned long now = millis();
    if (now - lastClickTime[4] < DEBOUNCE_DELAY) return;
    lastClickTime[4] = now;
    
    Serial.println("Toilet button clicked!");
    BleCommandManager::getInstance().toggleRelay(RELAY_MAP[4]);
}

void onBtnBathClick(lv_event_t * e)
{
    unsigned long now = millis();
    if (now - lastClickTime[5] < DEBOUNCE_DELAY) return;
    lastClickTime[5] = now;
    
    Serial.println("Bath button clicked!");
    BleCommandManager::getInstance().toggleRelay(RELAY_MAP[5]);
}

void onBtnBedClick(lv_event_t * e)
{
    unsigned long now = millis();
    if (now - lastClickTime[6] < DEBOUNCE_DELAY) return;
    lastClickTime[6] = now;
    
    Serial.println("Bed button clicked!");
    BleCommandManager::getInstance().toggleRelay(RELAY_MAP[6]);
}

void onBtnBalkonyUpClick(lv_event_t * e)
{
    unsigned long now = millis();
    if (now - lastClickTime[7] < DEBOUNCE_DELAY) return;
    lastClickTime[7] = now;
    
    Serial.println("Balkony Up button clicked!");
    BleCommandManager::getInstance().toggleRelay(RELAY_MAP[7]);
}

void onBtnBalkonyDownClick(lv_event_t * e)
{
    unsigned long now = millis();
    if (now - lastClickTime[8] < DEBOUNCE_DELAY) return;
    lastClickTime[8] = now;
    
    Serial.println("Balkony Down button clicked!");
    BleCommandManager::getInstance().toggleRelay(RELAY_MAP[8]);
}

void onBtnProjectorClick(lv_event_t * e)
{
    unsigned long now = millis();
    if (now - lastClickTime[9] < DEBOUNCE_DELAY) return;
    lastClickTime[9] = now;
    
    Serial.println("Projector button clicked!");
    BleCommandManager::getInstance().toggleRelay(RELAY_MAP[9]);
}

void onBtnReloadInformationClick(lv_event_t * e)
{
    Serial.println("Reload Information button clicked!");
    BleCommandManager::getInstance().requestStatus();
}

void onBtnCloseAllClick(lv_event_t * e)
{
    Serial.println("Close All button clicked!");
    BleCommandManager::getInstance().allRelaysOff();
}
